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

#define MAX_LOOKAHEAD (8192)
#define MASK 0x1FFF

// Global global variables

// Local global variables
static unsigned long interruptCounter;
static unsigned long processingCounter;
static unsigned long delay;
static unsigned long echo;
static unsigned long delayFilePos;
static unsigned long echoFilePos;
static FIL buttonFiles[2];
static button *buttons[2];
static FIL inputWrite;
static FIL inputRead;
static FIL outputWrite;
static FIL outputRead;
static buffer outputBuffer;

static int effects[2];
static int effectParam[2][2];

static signed short outputBuf[MAX_LOOKAHEAD];

static int writeIndex;
static int readIndex;

static button b1;
static button b2;

float a[3];
float b[3];

static char *files[NUM_BUTTONS] = {
	"guitar.dat",
	"sine.dat",
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





void load_init( void )
{
	interruptCounter = 0;
	delay = 0;
	echo = 0;
	delayFilePos = 44100 * 2;
	echoFilePos = 44100 * 2;

	writeIndex = 0;
	readIndex = 0;

	buffer_init( &outputBuffer );

	f_open( &inputWrite, "input.dat", FA_CREATE_ALWAYS | FA_WRITE );
	f_open( &inputRead, "input.dat", FA_CREATE_ALWAYS | FA_READ );
	f_open( &outputWrite, "output.dat", FA_CREATE_ALWAYS | FA_WRITE );
	f_open( &outputRead, "output.dat", FA_CREATE_ALWAYS | FA_READ );

	f_lseek( &inputWrite, 44100 * 2 );
	f_lseek( &inputRead, 44100 * 2 );
	f_lseek( &outputWrite, 44100 * 2 );
	f_lseek( &outputRead, 44100 * 2 );

	// Set buttons to be first two buttons
	load_set_one( 0 );
	load_set_two( 1 );
	b1.playTime = STOP_PLAYING;
	b2.playTime = STOP_PLAYING;

	//effect[0] = cfg.effectOne;
	//effect[1] = cfg.effectTwo;

	effects[0] = EFFECT_LOWPASS;
	effectParam[0][0] = 50;
	effectParam[0][1] = 10;

	generate_filter_coeffs(EFFECT_LOWPASS, 1000, 0.707, a, b);
}

signed short process( signed short input, signed short delay, signed short echo )
{
	int i = 0;
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

	for (i = 0; i < 2; i++)
	{
		switch (effects[i])
		{
			//TO DO: get rid of the overflow - if we are using 12 bit samples this shouldnt be a problem though i hope
			case EFFECT_LOWPASS:
			case EFFECT_HIGHPASS:
			case EFFECT_BANDPASS:
			case EFFECT_NOTCH:
				input = (input >> 2); //just to guard about overflow
				out = b[0]*input + b[1]*prevIn + b[2]*prevIn2 - a[1]*prevOut - a[2]*prevOut2;
				output += (signed short) out;
				prevIn2 = prevIn;
				prevIn = input;

				prevOut2 = prevOut;
				prevOut = output;
				break;
			case EFFECT_DELAY:
				break;
			case EFFECT_ECHO:
				break;
			case EFFECT_DECI_BIT:
				num = deci_y2 - deci_y1;
				denom = ((float)deciCount)/effectParam[i][1];

				interp = num/denom + deci_y1;
				output += ((signed short) interp);// >> 2;

				if (deciCount++ == effectParam[i][1])
				{
					//load in a new y2
					deci_y1 = deci_y2;
					deci_y2 = input;

					deciCount = 1;
				}

				//output += ((input/effectParam[i][0]) * effectParam[i][0]) >> 2; //crush

				break;
			case EFFECT_BITWISE:
				output += ((input & effectParam[i][0]) | effectParam[i][1]);
				break;
			default:
				//output += input >> 2;
		}

	}

	return output;
}

void push_output( signed short sample )
{
	outputBuf[writeIndex++] = sample;

	writeIndex &= MASK;

	//f_write( &outputWrite, &sample, 2, 0 );
}

int pushed = 0;

void push_input( signed short sample )
{
	f_write( &inputWrite, &sample, 2, 0 );

	if (pushed++ == 512)
	{
	f_sync(&inputWrite);
	pushed = 0;
	}
}

//not used atm
signed short pop_output( void )
{
	signed short val;
	buffer_read( &outputBuffer, &val, 1 );
	return val;
}

signed short pop_delay( void )
{
	if ( processingCounter > delay )
	{
		signed short val;
		f_read( &inputRead, &val, 2, 0 );
		delayFilePos += 2;
		return val;
	}
	return 0;
}
signed short pop_echo( void )
{
	if ( processingCounter > delay )
	{
		signed short val;
		f_read( &outputRead, &val, 2, 0 );
		echoFilePos += 2;
		return val;
	}
	return 0;
}


void load_set_one( unsigned long index )
{
	b1.mode = MODE_LATCH;
	b1.action = ACTION_EFFECT_NONE;
	b1.loopInterval = 0; //whats this even for
	b1.playTime = 0;
	b1.isLooped = 1;
	b1.interruptModulo = 50;

	// Set beat interval play time
	b1.playTime = interruptCounter + ( interruptCounter % b1.interruptModulo );

	// Set file pointer
	b1.fp = &(buttonFiles[index]);
	FRESULT res = f_open( b1.fp, files[index], FA_OPEN_EXISTING | FA_READ );

}

void load_set_two( unsigned long index )
{

	b2.mode = MODE_LATCH;
	b2.action = ACTION_EFFECT_NONE;
	b2.loopInterval = 0; //whats this even for
	b2.playTime = 0;
	b2.isLooped = 1;
	b2.interruptModulo = 50;


	// Set beat interval play time
	b2.playTime = interruptCounter + ( interruptCounter % b2.interruptModulo );

	// Set file pointer
	b2.fp = &(buttonFiles[index]);
	FRESULT res = f_open( b2.fp, files[index], FA_OPEN_EXISTING | FA_READ );
}

void load_set_delay( unsigned long samples )
{
	unsigned long offset = delayFilePos + 2 * ( delay - samples );
	delay = samples;
	f_lseek( &inputRead, offset );
	
}
void load_set_echo( unsigned long samples )
{
	unsigned long offset = delayFilePos + 2 * ( delay - samples );
	echo = samples;
	f_lseek( &inputRead, offset );
}

int i = 0;
void playback_interrupt( void )
{
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	SSIDataPut( SSI2_BASE, outputBuf[readIndex++] ); // Write Value
	readIndex &= MASK;

	interruptCounter++;
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0x00);
}

void do_work( void )
{
	FRESULT res;
	UINT read = 0;
	signed short temp = 0;
	signed short combined = 0;

	// Check if we are not too far ahead
	if ( processingCounter < interruptCounter + MAX_LOOKAHEAD )
	{
		if ( b1.playTime <= processingCounter ) // started now or in the past
		{
			temp = 0;

			res = f_read( b1.fp, &temp, 2, &read ); // this is auto buffered
			temp = (temp << 8) | ((temp >> 8) & 0x00FF);

			if ( read == 0 || res ) {
				if ( b1.isLooped ) {
					b1.playTime = interruptCounter + 1; //( interruptCounter % b1.interruptModulo );
					f_lseek(b1.fp, 0);
				} else {
					b1.playTime = STOP_PLAYING;
				}
			}
			combined += (temp >> 1);
		}

		if (b2.playTime <= processingCounter ) // started now or in the past
		{
			signed short temp = 0;

			res = f_read(b2.fp, &temp, 2, &read); // this is auto buffered

			temp = (temp << 8) | ((temp >> 8) & 0x00FF);

			if ( read == 0 || res ) {
				if ( b2.isLooped ) {
					b2.playTime = processingCounter + 1;
					f_lseek(b2.fp, 0);
				} else {
					b2.playTime = STOP_PLAYING;
				}
			}

			combined += (temp >> 1);
		}

		//push_input( combined );
		// Load sample into output
		signed short output = process( combined, 0, 0); //, pop_delay(), pop_echo() );

		push_output( output );

		processingCounter++;
	}
}

void test(void)
{
	load_init();

	load_set_one(0);
	load_set_two(1);

}

