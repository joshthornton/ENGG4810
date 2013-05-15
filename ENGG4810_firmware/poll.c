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
#include "load.h"
#include "poll.h"

#define OUTPUT_HERTZ (400)
#define TENTH_SEC (44100/10)

#define HIGH	(0xFF)
#define LOW		(0)

// Global global variables

unsigned long ledState[4][4];
unsigned long buttonState[4][4];

// Local file global variables
static unsigned long index;
static unsigned long lx, ly, bx, by;
static unsigned long trackOne;
static unsigned long trackTwo;



// LEDs
static const unsigned long LED_GROUND_PERIPHS[4] = { SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD };
static const unsigned long LED_GROUND_BASES[4] = { GPIO_PORTE_BASE, GPIO_PORTA_BASE, GPIO_PORTD_BASE, GPIO_PORTD_BASE };
static const unsigned long LED_GROUND_PINS[4] = { GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_2, GPIO_PIN_3 };
static const unsigned long LED_RED_PERIPHS[4] = { SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF };
static const unsigned long LED_RED_BASES[4] = { GPIO_PORTC_BASE, GPIO_PORTC_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE };
static const unsigned long LED_RED_PINS[4] = { GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_4, GPIO_PIN_3 };
static const unsigned long LED_GREEN_PERIPHS[4] = { SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOC };
static const unsigned long LED_GREEN_BASES[4] = { GPIO_PORTD_BASE, GPIO_PORTD_BASE, GPIO_PORTC_BASE, GPIO_PORTC_BASE };
static const unsigned long LED_GREEN_PINS[4] = { GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_7, GPIO_PIN_6 }; //i changed the first two pins cause they were shorted to dacs spi

// Buttons
static const unsigned long BTN_CHECK_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOB };
static const unsigned long BTN_CHECK_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTF_BASE, GPIO_PORTB_BASE, GPIO_PORTB_BASE };
static const unsigned long BTN_CHECK_PINS[4] = { GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_2, GPIO_PIN_3 };
static const unsigned long BTN_POWER_PERIPHS[4] = { SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD };
static const unsigned long BTN_POWER_BASES[4] = { GPIO_PORTF_BASE, GPIO_PORTE_BASE, GPIO_PORTD_BASE, GPIO_PORTD_BASE };
static const unsigned long BTN_POWER_PINS[4] = { GPIO_PIN_0, GPIO_PIN_5, GPIO_PIN_7, GPIO_PIN_6 };

static const unsigned long LOOP_PERIPH = SYSCTL_PERIPH_GPIOB;
static const unsigned long LOOP_BASE = GPIO_PORTB_BASE;
static const unsigned long LOOP_PIN = GPIO_PIN_1;


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
	trackOne = -1;
	trackTwo = -1;

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
	}
	SysCtlPeripheralEnable( LOOP_PERIPH );

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
		
		// Apparently this helps brightness
		GPIOPadConfigSet( LED_RED_BASES[i], LED_RED_PINS[i], GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD ); // Pull down
		GPIOPadConfigSet( LED_GREEN_BASES[i], LED_GREEN_PINS[i], GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD ); // Pull down
	}

	// Configure all inputs
	for ( i = 0; i < 4; ++i )
	{
		GPIOPinTypeGPIOInput( BTN_CHECK_BASES[i], BTN_CHECK_PINS[i] );

		// Apparently this helps prevent erroneous input
		GPIOPadConfigSet( BTN_CHECK_BASES[i], BTN_CHECK_PINS[i], GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD ); // Pull down
	}
	GPIOPinTypeGPIOInput( LOOP_BASE, LOOP_PIN ); 

	// Initialize LED ground high, button power low
	for ( i = 0; i < 4; ++i )
	{
		GPIOPinWrite( LED_GROUND_BASES[i], LED_GROUND_PINS[i], HIGH ); // Ground High
		GPIOPinWrite( BTN_POWER_BASES[i], BTN_POWER_PINS[i], LOW ); // Power low
	}

	// Setup Interrupt

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, ROM_SysCtlClockGet() / OUTPUT_HERTZ );
	ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerEnable(TIMER2_BASE, TIMER_A);
	ROM_IntEnable(INT_TIMER2A);

}

void start_playing( unsigned long index )
{
	// Check loop button
	unsigned long loop = GPIOPinRead( LOOP_BASE, LOOP_PIN );
	cfg.buttons[index].isLooped = ( loop != 0 );
	
	// Start track
	if ( index )
	{
		// Turn off 'third' track if currently playing
		if ( cfg.buttons[trackOne].playTime < interruptCounter )
			ledState[ trackOne % 4 ][ trackOne / 4 ] = LED_NONE;
		trackOne = index;
		load_set_one( index );
	}
	else
	{
		// Turn off 'third' track if currently playing
		if ( cfg.buttons[trackTwo].playTime < interruptCounter )
			ledState[ trackTwo % 4 ][ trackTwo / 4 ] = LED_NONE;
		trackTwo = index;
		load_set_two( index );
	}
		
	// Update index
	index = ( index + 1 ) % 2;
}

void button_pushed( unsigned long bx, unsigned long by )
{
	unsigned long index = bx * 4 + by;

	// Record press time
	buttonState[bx][by] = interruptCounter;
	
	// Three cases:
	//	1. HOLD and NOT PLAYING		-> start playing, led red
	//	2. LATCH and NOT PLAYING	-> start playing, led green
	//	3. LATCH and PLAYING		-> stop playing, led off
	// Set LED
	if ( cfg.buttons[index].mode == MODE_HOLD )
	{
		ledState[by][bx] = LED_RED;
		start_playing( index );
	} else {

		if ( cfg.buttons[index].playTime <= interruptCounter ) // Currently playing
		{
			cfg.buttons[index].playTime = STOP_PLAYING;
			ledState[by][bx] = LED_NONE;
		} else
		{
			ledState[by][bx] = LED_GREEN;
			start_playing( index );
		}
	}
}

void button_released( unsigned long bx, unsigned long by )
{
	unsigned long index = bx * 4 + by;

	// Ignore if very recent
	if ( buttonState[bx][by] + TENTH_SEC < interruptCounter )
	{
		// Reset state
		buttonState[bx][by] = 0;
		
		// Stop track if mode is hold
		if ( cfg.buttons[index].mode == MODE_HOLD )
		{
				cfg.buttons[index].playTime = STOP_PLAYING;
				ledState[by][bx] = LED_NONE;
		}

	}
}

void poll_interrupt( void )
{

	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

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
			button_pushed( bx, by );
		else if ( !val && buttonState[bx][by] )
			button_released( bx, by );
	}
	GPIOPinWrite( BTN_POWER_BASES[bx], BTN_POWER_PINS[bx], LOW ); // Power up button

	// Update button index
	bx++;
	if ( bx == 4 )
		bx = 0;
}
