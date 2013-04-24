#include "poll.h"
#include "config.h"
#include "load.h"

#define OUTPUT_HERTZ (1600)

#define HIGH	(1)
#define LOW		(0)

// Global global variables
unsigned long effectValues[4];
unsigned long ledState;
unsigned long buttonState;

// Local file global variables
static unsigned long counter;

// LEDs
static const unsigned long LED_GROUND_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long LED_GROUND_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long LED_GROUND_PINS[4] = { GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1 }; 
static const unsigned long LED_RED_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long LED_RED_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long LED_RED_PINS[4] = { GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1 }; 
static const unsigned long LED_GREEN_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long LED_GREEN_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long LED_GREEN_PINS[4] = { GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1 }; 

// Buttons
static const unsigned long BTN_POWER_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long BTN_POWER_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long BTN_POWER_PINS[4] = { GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1 }; 
static const unsigned long BTN_CHECK_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long BTN_CHECK_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long BTN_CHECK_PINS[4] = { GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1, GPIO_PIN_1 }; 

// Linear Potentiometers
static const unsigned long LINEAR_ADC_PERIPHS[4] = {};
static const unsigned long LINEAR_GPIO_PERIPHS[4] = {};
static const unsigned long LINEAR_BASES[4] = {};
static const unsigned long LINEAR_PINS[4] = {};

void poll_init( void )
{

	// Set state
	ledState = 0;
	buttonState = 0;

	// Enable all peripherals
	for ( int i = 0; i < 4; ++i )
	{
		 SysCtlPeripheralEnable( LED_GROUND_PERIPHS[i] );		
		 SysCtlPeripheralEnable( LED_RED_PERIPHS[i] );		
		 SysCtlPeripheralEnable( LED_GREEN_PERIPHS[i] );		
		 SysCtlPeripheralEnable( BTN_POWER_PERIPHS[i] );		
		 SysCtlPeripheralEnable( BTN_CHECK_PERIPHS[i] );		
		 SysCtlPeripheralEnable( LINEAR_ADC_PERIPHS[i] );		
		 SysCtlPeripheralEnable( LINEAR_GPIO_PERIPHS[i] );		
	}

	// Configure all outputs
	for ( int i = 0; i < 4 ++i )
	{
		GPIOPinTypeGPIOOutput( LED_GROUND_BASES[i], LED_GROUND_PINS[i] );
		GPIOPinTypeGPIOOutput( LED_RED_BASES[i], LED_RED_PINS[i] );
		GPIOPinTypeGPIOOutput( LED_GREEN_BASES[i], LED_GREEN_PINS[i] );
		GPIOPinTypeGPIOOutput( BTN_POWER_BASES[i], BTN_POWER_PINS[i] );
	}

	// Configure all inputs
	for ( int i = 0; i < 4; ++i )
	{
		GPIOPinTypeGPIOInput( BTN_CHECK_BASES[i], BTN_CHECK_PINS[i] );
	}

	// Configure ADC 
	for ( int i = 0; i < 4; ++i )
	{
		MAP_GPIOPinTypeADC( LINEAR_BASES[i], LINEAR_PINS[i] );
	}

	// Setup Interrupt
	counter = 0;
	ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER0 );
	ROM_TimerConfigure( TIMER0_BASE, TIMER_CFG_PERIODIC );
	ROM_TimerLoadSet( TIMER0_BASE, TIMER_B, ROM_SysCtlClockGet() / hertz );
	ROM_IntEnable( INT_TIMER0B );
	ROM_TimerIntEnable( TIMER0_BASE, TIMER_TIMB_TIMEOUT );
	ROM_TimerEnable( TIMER0_BASE, TIMER_B );

}

void poll_interrupt( void )
{

	// Get current LED
	unsigned long mod = counter % 16; 
	unsigned long x = mod / 4; 
	unsigned long y = mod % 4;

	// Switch off previous LED
	GPIOPinWrite( LED_GROUND_BASE[x], LED_GROUND_PIN[x], HIGH ); // Ground High 
	GPIOPinWrite( LED_RED_BASE[y], LED_RED_PIN[y], LOW ); // Red Low
	GPIOPinWrite( LED_GREEN_BASE[y], LED_GREEN_PIN[y], LOW ); // Green Low

	// Get next LED
	mod = ( counter + 1 ) % 16;
	x = mod / 4 ;
	y = mod % 4;

	// Switch on next LED
	GPIOPinWrite( LED_GROUND_BASE[x], LED_GROUND_PIN[x], LOW ); // Ground Low 
	if ( LED_GET( mod ) & LED_RED )
		GPIOPinWrite( LED_RED_BASE[y], LED_RED_PIN[y], HIGH ); // Red High
	if ( LED_GET( mod ) & LED_GREEN )
		GPIOPinWrite( LED_GREEN_BASE[y], LED_GREEN_PIN[y], HIGH ); // Green High

	
	// Check a button
	GPIOPinWrite( BTN_POWER_BASE[x], BTN_POWER_PIN[x], HIGH ); // Power up button
	unsigned long val = GPIOPinRead( BTN_CHECK_BASE[y], BTN_CHECK_PIN[y] ); // Check button
	if ( val && !BTN_GET( mod ) )
	{
		BTN_SET( mod, 1 );
		if ( index == 0 )
			load_set_zero( mod );
		else
			load_set_one( mod );
		index = ( index + 1 ) % 2
	} else if ( !val ) {
		BTN_SET( mod, 0 );
	}
	GPIOPinWrite( BTN_POWER_BASE[x], BTN_POWER_PIN[x], LOW ); // Power up button

	// Check an ADC
	

	// Increment counter
	counter++;
}
