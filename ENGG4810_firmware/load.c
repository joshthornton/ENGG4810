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
#define DELAY_LENGTH  (4096)
#define MASK 0x1FFF
#define DELAY_MASK 0xFFF
#define NUM_LEVELS		(128.0f)
#define FREQ_OFFSET_LP	    (800.0f)
#define FREQ_OFFSET_HP  (0.0f)
#define FREQ_GRADIENT_LP	( (20000.0f-FREQ_OFFSET_LP) / NUM_LEVELS )
#define FREQ_GRADIENT_HP	( (7500.0f-FREQ_OFFSET_HP) / NUM_LEVELS )
#define Q_GRADIENT		( 1.0f / NUM_LEVELS )
#define Q_OFFSET		(0.0f)
#define SIZE_GRADIENT	( BUFFER_SIZE / NUM_LEVELS )

// Global global variables

// Local global variables
unsigned long interruptCounter;
static unsigned long processingCounter;
static FIL buttonFiles[2];

static int effects[2];
static int effectParam[2][2];

static float echoAmount = 0.0f;
static float delayAmount = 0.0f;

static signed short outputBuf[MAX_LOOKAHEAD];
static signed short delayBuf[DELAY_LENGTH]; //also used for echo

static int delayWriteIndex;
static int delayReadIndex;

static int writeIndex;
static int readIndex;

static volatile button *b1;
static volatile button *b2;

float a_1[3];
float b_1[3];

float a_2[3];
float b_2[3];

static const unsigned long TEMPO_PERIPH = SYSCTL_PERIPH_GPIOA;
static const unsigned long TEMPO_BASE = GPIO_PORTA_BASE;
static const unsigned long TEMPO_PIN = GPIO_PIN_7;

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

int deciCount = 1;

signed short deci_y1 = 0;
signed short deci_y2 = 0;

void load_set_params(unsigned long effect, unsigned long param1, unsigned long param2)
{
	effectParam[effect][0] = param1;
	effectParam[effect][1] = param2;
	signed short a;
	signed short b;
	float Fc;
	float Q;

	switch (effects[effect])
	{
		case EFFECT_LOWPASS:
		case EFFECT_HIGHPASS:
		case EFFECT_BANDPASS:
		case EFFECT_NOTCH:

			if (effects[effect] == EFFECT_LOWPASS)
			{
				Fc = ((float) param1) * FREQ_GRADIENT_LP + FREQ_OFFSET_LP;
			} else
			{
				Fc = ((float) param1) * FREQ_GRADIENT_HP + FREQ_OFFSET_HP;
			}

			Q =  ( param2 ) * Q_GRADIENT + Q_OFFSET;

			load_generate_coeffs(effect, effects[effect], (int)Fc, Q);
			break;
		case EFFECT_DELAY:
			delayReadIndex = (delayWriteIndex - (param1 << 5)) % DELAY_LENGTH; //just in case it gets too high
			delayAmount = param2/(128.0f);
			break;
		case EFFECT_ECHO:
			delayReadIndex = (delayWriteIndex - (param1 << 5)) % DELAY_LENGTH; //just in case it gets too high
			echoAmount = param2/(256.0f);
			break;
		case EFFECT_DECI_BIT:
			deciCount = 1;

			deci_y1 = 0;
			deci_y2 = 0;
			effectParam[effect][1] = param2 + 1;// << 5;
			effectParam[effect][0] = param1 << 3;
			break;
		case EFFECT_BITWISE:
			a = (param1 - 64)*512;
			b = (param2 - 64)*512;

			effectParam[effect][0] = a;
			effectParam[effect][1] = b;
			break;

		default:
			break;
	}
}


signed short prevIn_1 = 0;
signed short prevIn2_1 = 0;
signed short prevOut_1 = 0;
signed short prevOut2_1 = 0;

signed short prevIn_2 = 0;
signed short prevIn2_2 = 0;
signed short prevOut_2 = 0;
signed short prevOut2_2 = 0;

void load_generate_coeffs(int index, int effect, int Fc, float Q)
{
	if (index == 0) {
		prevIn_1 = 0;
		prevIn2_1 = 0;
		prevOut_1 = 0;
		prevOut2_1 = 0;
		generate_filter_coeffs(effect, Fc, Q, a_1, b_1);
	}
	else {

		prevIn_2 = 0;
		prevIn2_2 = 0;
		prevOut_2 = 0;
		prevOut2_2 = 0;
		generate_filter_coeffs(effect, Fc, Q, a_2, b_2);
	}

}

void load_init( void )
{
	interruptCounter = 0;

	writeIndex = 0;
	readIndex = 0;
	delayWriteIndex = 0;
	delayReadIndex = 0;

	load_generate_coeffs( 0, 1, 4000, 0.707 );
	cfg.effectOne = EFFECT_ECHO;
	cfg.effectTwo = EFFECT_LOWPASS;
	//load_generate_coeffs( 0, 0, 10000, 0.707 );
	//buffer_init( &inBuf );
	//buffer_init( &outBuf );

	// Set buttons to be first two buttons
	load_set_one( 0 );
	load_set_two( 1 );
	b1->playTime = STOP_PLAYING;
	b2->playTime = STOP_PLAYING;

	effects[0] = cfg.effectOne;
	effects[1] = cfg.effectTwo;
	SysCtlPeripheralEnable( TEMPO_PERIPH );
	GPIOPinTypeGPIOOutput(TEMPO_BASE, TEMPO_PIN);

	GPIOPadConfigSet( TEMPO_BASE, TEMPO_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU ); // Pull down

}

void load_set_one( unsigned long index )
{

	b1 = &(cfg.buttons[index]);

	if (b1 == b2)
		b2 = NULL;

	// Set beat interval play time
	b1->playTime = interruptCounter + ( interruptCounter % b1->interruptModulo );

	b1->fp = NULL;

}

void load_set_two( unsigned long index )
{
	b2 = &(cfg.buttons[index]);
	
	if (b2 == b1)
		b1 = NULL;

	// Set beat interval play time
	b2->playTime = interruptCounter + ( interruptCounter % b2->interruptModulo );

	b2->fp = NULL;
}

void playback_interrupt( void )
{
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	//char lower = (char)(i&0x00FF);
	//char upper = (char)((i >> 8)&0x00FF);

	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0xFF);
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0x00);

	SSIDataPut( SSI2_BASE, outputBuf[readIndex++]); // Write Value

	//while( SSIBusy( SSI2_BASE ) ); //output
	//
	static unsigned long pinOn = 0xFF;

	if ((interruptCounter % cfg.bpm) == 0)
	{
		GPIOPinWrite(TEMPO_BASE, TEMPO_PIN, pinOn);
		pinOn = ~pinOn;
	}

	GPIOPinWrite(TEMPO_BASE, TEMPO_PIN, 0xFF);

	//myval += 1000;
	readIndex &= MASK;
	interruptCounter++;
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0x00);
}

void do_work( void )
{
	FRESULT res;
	WORD read = 0;
	signed short temp = 0;
	signed short input = 0;
	signed short output = 0;

	float num;
	float denom;
	float interp;
	float out;

	// Check if we are not too far ahead
	if ( processingCounter < interruptCounter + MAX_LOOKAHEAD )
	{
		if (b1 && b1->playTime <= processingCounter) // started now or in the past
		{
			if ( !(b1->fp) )
			{
				// Setup to reuse file pointer
				b1->fp = &(buttonFiles[ 0 ]);

				// Open file
				FRESULT res = f_open( b1->fp, files[ b1->index ], FA_OPEN_EXISTING | FA_READ );
			}

			temp = 0;

			res = f_read( b1->fp, &temp, 2, &read );

			if ( read == 0 || res ) {	// End of file
				if ( b1->isLooped ) {
					b1->playTime = interruptCounter + ( interruptCounter % b1->interruptModulo ); // start on next interval
					f_lseek(b1->fp, 0); // seek back to start of file
				} else {
					b1->playTime = STOP_PLAYING; // buttons and leds will check this on next interrupt and update accordingly
					b1->fp = 0;
				}
			}
			input += temp >> 4;
		}

		if (b2 && b2->playTime <= processingCounter) // started now or in the past
		{

			if ( !(b2->fp) )
			{
				// Setup to reuse file pointer
				b2->fp = &(buttonFiles[ 1 ]);

				// Open file
				FRESULT res = f_open( b2->fp, files[b2->index], FA_OPEN_EXISTING | FA_READ );
			}

			signed short temp = 0;

			res = f_read(b2->fp, &temp, 2, &read);

			if ( read == 0 || res ) {
				if ( b2->isLooped ) {
					b2->playTime = processingCounter + ( interruptCounter % b2->interruptModulo );
					f_lseek(b2->fp, 0);
				} else {
					b2->playTime = STOP_PLAYING;
					b2->fp = 0;
				}
			}

			input += temp >> 4;
		}

		// Save input for delay
		//buffer_push( &inBuf, input );

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
					if (i == 0)
					{
						out = b_1[0]*input + b_1[1]*prevIn_1 + b_1[2]*prevIn2_1 - a_1[1]*prevOut_1 - a_1[2]*prevOut2_1;

						prevIn2_1 = prevIn_1; //& 0x9FFF;
						prevIn_1 = input; //& 0x9FFF;

						prevOut2_1 = prevOut_1; //& 0x9FFF;
						prevOut_1 = (signed short) out;//& 0x9FFF;

						output += prevOut_1 >> 1;

					} else
					{
						out = b_2[0]*input + b_2[1]*prevIn_2 + b_2[2]*prevIn2_2 - a_2[1]*prevOut_2 - a_2[2]*prevOut2_2;

						prevIn2_2 = prevIn_2; //& 0x9FFF;
						prevIn_2 = input; //& 0x9FFF;

						prevOut2_2 = prevOut_2; //& 0x9FFF;
						prevOut_2 = (signed short) out;//& 0x9FFF;

						output += prevOut_2 >> 1;
					}
					break;

				case EFFECT_DELAY:
					output += ((signed short)(input + delayAmount * delayBuf[delayReadIndex++])) << 1;
					delayReadIndex &= DELAY_MASK;
					break;

				case EFFECT_ECHO:
					output += ((signed short)(input + echoAmount * delayBuf[delayReadIndex++])) << 1;
					delayReadIndex &= DELAY_MASK;
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
					output += ((input & effectParam[i][0]) | effectParam[i][1]) >> 1;
					break;

				case EFFECT_NONE:
					output += output >> 1; //one effect is applied
					break;
				default:
					output += input >> 1; //theres nothing applied at all
			}

		}

		if (effects[0] == EFFECT_DELAY || effects[1] == EFFECT_DELAY)
		{
			delayBuf[delayWriteIndex++] = input;
			delayWriteIndex &= DELAY_MASK;
		} else if (effects[0] == EFFECT_ECHO || effects[1] == EFFECT_ECHO)
		{
			delayBuf[delayWriteIndex++] = output;
			delayWriteIndex &= DELAY_MASK;
		}
		//output = input;

		// Save output for echo
		//buffer_push( &outBuf, output );

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

