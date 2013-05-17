#include "load.h"
#include "config.h"
#include "buffer.h"
#include "third_party/fatfs/src/ff.h"
#include "dac.h"
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
#include <math.h>
#include "arm_math.h"
#include "filterGen.h"
#include "config.h"

#define MAX_LOOKAHEAD (8192)
#define MASK 0x1FFF

// Global global variables

// Local global variables
unsigned long interruptCounter;
static unsigned long processingCounter;
static FIL buttonFiles[2];

static int effects[2];
static int effectParam[2][2];

// For echo and delay
static buffer outBuf;
static buffer inBuf;
static float echoAmount = 0.0f;
static float delayAmount = 0.0f;

static signed short outputBuf[MAX_LOOKAHEAD];
static signed short delayBuf[MAX_LOOKAHEAD];

static int writeIndex;
static int readIndex;

static button *b1;
static button *b2;

float a[3];
float b[3];

static char *files[NUM_BUTTONS] = {
	"1.dat",
	"2.dat",
	"3.dat",
	"4.dat",
	"5.dat",
	"6.dat",
	"7.dat",
	"8.dat",
	"9.dat",
	"10.dat",
	"11.dat",
	"12.dat",
	"13.dat",
	"14.dat",
	"15.dat",
	"16.dat"
};

void load_set_delay( unsigned long size, float amount )
{
	delayAmount = amount;
	buffer_set_size( &inBuf, size );
}

void load_set_echo( unsigned long size, float amount )
{
	echoAmount = amount;
	buffer_set_size( &outBuf, size );
}

void load_generate_coeffs(int effect, int Fc, float Q)
{
	generate_filter_coeffs(effect, Fc, Q, a, b);
}

void load_init( void )
{
	interruptCounter = 0;

	writeIndex = 0;
	readIndex = 0;

	buffer_init( &inBuf );
	buffer_init( &outBuf );

	// Set buttons to be first two buttons
	load_set_one( 0 );
	load_set_two( 1 );
	b1->playTime = STOP_PLAYING;
	b2->playTime = STOP_PLAYING;

	effect[0] = cfg.effectOne;
	effect[1] = cfg.effectTwo;

}

void load_set_one( unsigned long index )
{
	b1 = &(cfg.buttons[index]);

	// Set beat interval play time
	b1->playTime = interruptCounter + ( interruptCounter % b1->interruptModulo );

	// Setup to reuse file pointer 
	b1->fp = &(buttonFiles[ 0 ]);

	// Open file
	FRESULT res = f_open( b1->fp, files[index], FA_OPEN_EXISTING | FA_READ );

}

void load_set_two( unsigned long index )
{
	b2 = &(cfg.buttons[index]);
	
	// Set beat interval play time
	b2->playTime = interruptCounter + ( interruptCounter % b2->interruptModulo );

	// Setup to reuse file pointer
	b2->fp = &(buttonFiles[ 1 ]);

	// Open file
	FRESULT res = f_open( b2->fp, files[index], FA_OPEN_EXISTING | FA_READ );
}

int i = 0;
void playback_interrupt( void )
{
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	SSIDataPut( SSI2_BASE, (outputBuf[readIndex++]));// outputBuf[readIndex++] ); // Write Value

	readIndex &= MASK;

	interruptCounter++;
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0x00);
}

void do_work( void )
{
	FRESULT res;
	UINT read = 0;
	signed short temp = 0;
	signed short input = 0;
	signed short output = 0;
	static int deciCount = 1;

	static signed short deci_y1 = 0;
	static signed short deci_y2 = 0;

	float num;
	float denom;
	float interp;
	float out;

	static signed short prevIn = 0;
	static signed short prevIn2 = 0;
	static signed short prevOut = 0;
	static signed short prevOut2 = 0;

	// Check if we are not too far ahead
	if ( processingCounter < interruptCounter + MAX_LOOKAHEAD )
	{
		if ( b1->playTime <= processingCounter ) // started now or in the past
		{
			temp = 0;

			res = f_read( b1->fp, &temp, 2, &read );

			if ( read == 0 || res ) {	// End of file
				if ( b1->isLooped ) {
					b1->playTime = interruptCounter + ( interruptCounter % b1->interruptModulo ); // start on next interval
					f_lseek(b1->fp, 0); // seek back to start of file
				} else {
					b1->playTime = STOP_PLAYING; // buttons and leds will check this on next interrupt and update accordingly
				}
			}
			input += temp >> 4;
		}

		if (b2->playTime <= processingCounter ) // started now or in the past
		{
			signed short temp = 0;

			res = f_read(b2->fp, &temp, 2, &read);

			if ( read == 0 || res ) {
				if ( b2->isLooped ) {
					b2->playTime = processingCounter + ( interruptCounter % b2->interruptModulo );
					f_lseek(b2->fp, 0);
				} else {
					b2->playTime = STOP_PLAYING;
				}
			}

			input += temp >> 4;
		}

		// Save input for delay
		buffer_push( &inBuf, input );

		// would it be worth checking if input = 0 and having a different return case?
		int i = 0;
		for (i = 0; i < 2; i++)
		{
			switch (effects[i])
			{
				//TO DO: get rid of the overflow - if we are using 12 bit samples this shouldnt be a problem though i hope
				case EFFECT_LOWPASS:
				case EFFECT_HIGHPASS:
				case EFFECT_BANDPASS:
				case EFFECT_NOTCH:
					out = b[0]*input + b[1]*prevIn + b[2]*prevIn2 - a[1]*prevOut - a[2]*prevOut2;
					output += (signed short) out;
					prevIn2 = prevIn;
					prevIn = input;

					prevOut2 = prevOut;
					prevOut = output;
					break;
				case EFFECT_DELAY:
					output = input + delayAmount * buffer_pop( &inBuf ); 
					break;
				case EFFECT_ECHO:
					output = input + echoAmount * buffer_pop( &outBuf ); 
					break;
				case EFFECT_DECI_BIT:
					num = deci_y2 - deci_y1;
					denom = ((float)deciCount)/effectParam[i][1];

					interp = num/denom + deci_y1;
					output += ((signed short) interp) >> 2;

					if (deciCount++ == effectParam[i][1])
					{
						//load in a new y2
						deci_y1 = deci_y2;
						deci_y2 = input;

						deciCount = 1;
					}

					output += ((input/effectParam[i][0]) * effectParam[i][0]) >> 2; //crush

					break;
				case EFFECT_BITWISE:
					output += ((input & effectParam[i][0]) | effectParam[i][1]);
					break;
				default:
					//output += input >> 2;
			}

		}

		// Save output for echo
		buffer_push( &outBuf, ouput );

		outputBuf[writeIndex++] = output;



		writeIndex &= MASK;

		processingCounter++;





	}
}

void test(void)
{
	load_init();

	load_set_one(0);
	load_set_two(1);

}

