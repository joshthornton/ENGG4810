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
#include "inc/hw_gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"

#include "poll.h"

#define OUTPUT_HERTZ (16000)

#define HIGH	(0xFF)
#define LOW		(0)

// Global global variables

int ledState[4][4];
int buttonState[4][4];

// Local file global variables
unsigned long index;
unsigned long lx, ly, bx, by;


// LEDs
static const unsigned long LED_GROUND_PERIPHS[4] = { SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD };
static const unsigned long LED_GROUND_BASES[4] = { GPIO_PORTE_BASE, GPIO_PORTA_BASE, GPIO_PORTD_BASE, GPIO_PORTD_BASE };
static const unsigned long LED_GROUND_PINS[4] = { GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_2, GPIO_PIN_3 }; 
static const unsigned long LED_RED_PERIPHS[4] = { SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long LED_RED_BASES[4] = { GPIO_PORTC_BASE, GPIO_PORTC_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long LED_RED_PINS[4] = { GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_4, GPIO_PIN_3 }; 
static const unsigned long LED_GREEN_PERIPHS[4] = { SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOC };
static const unsigned long LED_GREEN_BASES[4] = { GPIO_PORTD_BASE, GPIO_PORTD_BASE, GPIO_PORTC_BASE, GPIO_PORTC_BASE };
static const unsigned long LED_GREEN_PINS[4] = { GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_7, GPIO_PIN_6 }; 

// Buttons
static const unsigned long BTN_CHECK_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOB };
static const unsigned long BTN_CHECK_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTB_BASE, GPIO_PORTB_BASE };
static const unsigned long BTN_CHECK_PINS[4] = { GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_2, GPIO_PIN_3 };
static const unsigned long BTN_POWER_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD };
static const unsigned long BTN_POWER_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTE_BASE, GPIO_PORTD_BASE, GPIO_PORTD_BASE };
static const unsigned long BTN_POWER_PINS[4] = { GPIO_PIN_0, GPIO_PIN_5, GPIO_PIN_7, GPIO_PIN_6 };

// Linear Potentiometers
//static const unsigned long LINEAR_ADC_PERIPHS[4] = {};
//static const unsigned long LINEAR_GPIO_PERIPHS[4] = {};
//static const unsigned long LINEAR_BASES[4] = {};
//static const unsigned long LINEAR_PINS[4] = {};

void poll_init( void )
{
	int i, j;
	// Set state
	lx = ly = bx = by = 0;
	index = 0;

	// Default buttons and leds
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ ) {
			ledState[i][j] = 0;
			buttonState[i][j] = 0;
		}

	// Enable all peripherals
	for ( i = 0; i < 4; ++i )
	{
		 SysCtlPeripheralEnable( LED_GROUND_PERIPHS[i] );		
		 SysCtlPeripheralEnable( LED_RED_PERIPHS[i] );		
		 SysCtlPeripheralEnable( LED_GREEN_PERIPHS[i] );		
		 SysCtlPeripheralEnable( BTN_POWER_PERIPHS[i] );		
		 SysCtlPeripheralEnable( BTN_CHECK_PERIPHS[i] );		
		 //SysCtlPeripheralEnable( LINEAR_ADC_PERIPHS[i] );
		 //SysCtlPeripheralEnable( LINEAR_GPIO_PERIPHS[i] );
	}

	// Unlock Registers
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x80;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

	// Configure all outputs
	for ( i = 0; i < 4; ++i )
	{
		GPIOPinTypeGPIOOutput( LED_GROUND_BASES[i], LED_GROUND_PINS[i] );
		GPIOPinTypeGPIOOutput( LED_RED_BASES[i], LED_RED_PINS[i] );
		GPIOPinTypeGPIOOutput( LED_GREEN_BASES[i], LED_GREEN_PINS[i] );
		GPIOPinTypeGPIOOutput( BTN_POWER_BASES[i], BTN_POWER_PINS[i] );
	}

	// Configure all inputs
	for ( i = 0; i < 4; ++i )
	{
		GPIOPinTypeGPIOInput( BTN_CHECK_BASES[i], BTN_CHECK_PINS[i] );
	}

	// Initialize LED ground high, button power low
	for ( i = 0; i < 4; ++i )
	{
		GPIOPinWrite( LED_GROUND_BASES[i], LED_GROUND_PINS[i], HIGH ); // Ground High
		GPIOPinWrite( BTN_POWER_BASES[i], BTN_POWER_PINS[i], LOW ); // Power low
	}

	// Configure ADC 
	//for ( int i = 0; i < 4; ++i )
	//{
	//	MAP_GPIOPinTypeADC( LINEAR_BASES[i], LINEAR_PINS[i] );
	//}

	// Setup Interrupt
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet() / OUTPUT_HERTZ );
	ROM_IntEnable(INT_TIMER0A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);

}

void poll_interrupt( void )
{

	// Switch off previous LED
	GPIOPinWrite( LED_GROUND_BASES[lx], LED_GROUND_PINS[lx], HIGH ); // Ground High
	GPIOPinWrite( LED_RED_BASES[ly], LED_RED_PINS[ly], LOW ); // Red Low
	GPIOPinWrite( LED_GREEN_BASES[ly], LED_GREEN_PINS[ly], LOW ); // Green Low

	// get next row / col
	unsigned long i = 16;
	do {
		ly++;
		if ( ly == 4 ) {
			lx++;
			ly = 0;
		}
		if ( lx == 4 )
			lx = 0;
	} while( !ledState[lx][ly]  && i-- );

	// Switch on next LED
	GPIOPinWrite( LED_GROUND_BASES[lx], LED_GROUND_PINS[lx], LOW ); // Ground Low
	if ( ledState[lx][ly] & LED_RED ) {
		GPIOPinWrite( LED_RED_BASES[ly], LED_RED_PINS[ly], HIGH ); // Red High
	}
	if ( ledState[lx][ly] & LED_GREEN ) {
		GPIOPinWrite( LED_GREEN_BASES[ly], LED_GREEN_PINS[ly], HIGH ); // Green High
	}
	
	// Check one button in each row button
	GPIOPinWrite( BTN_POWER_BASES[bx], BTN_POWER_PINS[bx], HIGH ); // Power up button
	for ( by = 0; by < 4; by++ ) {
		unsigned long val = GPIOPinRead( BTN_CHECK_BASES[by], BTN_CHECK_PINS[by] ); // Check button
		if ( val && !buttonState[bx][by] )
		{
			UARTprintf("button: %d, %d\n", bx,by);
			buttonState[bx][by] = 1;
			ledState[by][bx] = LED_GREEN;
			index = ( index + 1 ) % 2;
		} else if ( !val ) {
			buttonState[bx][by] = 0;
			ledState[by][bx] = 0;
		}
	}
	GPIOPinWrite( BTN_POWER_BASES[bx], BTN_POWER_PINS[bx], LOW ); // Power up button

	bx++;
	if ( bx == 4 )
		bx = 0;

	// Check an ADC
}

