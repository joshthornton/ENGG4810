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

#define DUMMY (0x0000)
#define IO_READ (0x41)
#define IO_WRITE (0x40)
#define A_MASK (0x00)
#define B_MASK (0x10)
#define IODIR (0x00)

void dac_write(signed long val )
{
	SSIDataPut( SSI2_BASE, val ); // Write Value
	while( ROM_SSIBusy( SSI2_BASE ) );
}

void Timer0IntHandler(void)
{

	static long i = 0;
	static signed long val = 0;
	static signed long val1 = 0;
	static signed long val2 = 0;
	//static int dir = 0;

    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //HWREGBITW(&g_ulFlags, 0) ^= 1;


    //val1 = buffer[buffNum][i] << 8;
    //i++;
    //val2 = buffer[buffNum][i];
    //val = val1 | val2;

    val = i;

    dac_write(val);

    i++;

    if (i >= 4096)
    {
    	//buffNum = !buffNum;
    	i = 0;
    }


    ROM_IntMasterDisable();
    ROM_IntMasterEnable();
}

void InitDACSSI(void)
{
	unsigned long temp;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PB4_SSI2CLK); // pin 5
	GPIOPinConfigure(GPIO_PB5_SSI2FSS); // pin 6
	GPIOPinConfigure(GPIO_PB6_SSI2RX);
	GPIOPinConfigure(GPIO_PB7_SSI2TX); //pin 7
	//GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 ); //latch

	GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5  | GPIO_PIN_6 | GPIO_PIN_7);

	SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, // Falling edge clock == 3
	  SSI_MODE_MASTER, 10000000, 16); // 10MHz clock

	SSIEnable(SSI2_BASE);

	while(SSIDataGetNonBlocking(SSI2_BASE, &temp));
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

void init_dac()
{
	InitDACSSI();
	InitTimer0( 48000 );
}

