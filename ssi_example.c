#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"

#define EVER ;;
#define DUMMY (0x00)
#define IO_READ (0x41)
#define IO_WRITE (0x40)
#define A_MASK (0x00)
#define B_MASK (0x10)
#define IODIR (0x00)

unsigned long g_ulFlags;
unsigned long i;

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

void mcp23018_write( unsigned long reg, unsigned long val )
{
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x00 ); // CS Low

	SSIDataPut( SSI0_BASE, IO_WRITE ); // Address
	while( ROM_SSIBusy( SSI0_BASE ) );

	SSIDataPut( SSI0_BASE, reg ); // Register
	while( ROM_SSIBusy( SSI0_BASE ) );

	SSIDataPut( SSI0_BASE, val ); // Write Value
	while( ROM_SSIBusy( SSI0_BASE ) );

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3 ); // CS High
}

unsigned long mcp23018_read( unsigned long reg )
{
	unsigned long temp;

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x00 ); // CS Low

	SSIDataPut( SSI0_BASE, IO_READ ); // Address
	while( ROM_SSIBusy( SSI0_BASE ) );

	SSIDataPut( SSI0_BASE, reg ); // Register
	while( ROM_SSIBusy( SSI0_BASE ) );

	SSIDataPut( SSI0_BASE, DUMMY ); // Read Value
	while( ROM_SSIBusy( SSI0_BASE ) );
	SSIDataGet( SSI0_BASE, &temp );
	while( ROM_SSIBusy( SSI0_BASE ) );

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3 ); // CS High

	return temp;
}

void InitConsole(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);
}

void
Timer0IntHandler(void)
{
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    HWREGBITW(&g_ulFlags, 0) ^= 1;

    mcp23018_write( A_MASK | IODIR, 0xAA );
    unsigned long a = mcp23018_read( A_MASK | IODIR );
    UARTprintf("a: 0x%x\n", a );

    mcp23018_write( B_MASK | IODIR, 0x55 );
	unsigned long b = mcp23018_read( B_MASK | IODIR );
	UARTprintf("b: 0x%x\n", b );

    ROM_IntMasterDisable();
    ROM_IntMasterEnable();
}

void InitLED(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1);
}

void InitSSI(void)
{
	unsigned long temp;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 );

	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 /*| GPIO_PIN_3*/ | GPIO_PIN_2);

	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, // Falling edge clock == 3
	  SSI_MODE_MASTER, 10000000, 8); // 10MHz clock

	SSIEnable(SSI0_BASE);

	while(SSIDataGetNonBlocking(SSI0_BASE, &temp));
}

void InitTimer0( unsigned long hertz )
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet() / hertz );
	ROM_IntEnable(INT_TIMER0A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

int
main(void)
{
    // Interrupt Setup
    ROM_FPULazyStackingEnable();
    ROM_IntMasterEnable();

    // 80MHz Clock
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    InitSSI();
    InitConsole();
    InitTimer0( 10 );

    for( EVER );
}

