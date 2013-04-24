#include "load.h"
#include "config.h"
#include "buffer.h"

#define MAX_LOOKAHEAD (500)

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

static const char *files[NUM_BUTTONS] = {};

void playback_interrupt( void )
{
	dac_write( pop_output() );
	interruptCounter++;
}

void load_init( void )
{
	interruptCounter = 0;
	delay = 0;
	echo = 0;
	delayFilePos = 44100 * 2;
	echoFilePos = 44100 * 2;

	buffer_init( &outputBuffer );

	f_open( &inputWrite, "input.dat", FA_CREATE_ALWAYS | FA_WRITE );
	f_open( &inuptRead, "input.dat", FA_CREATE_ALWAYS | FA_READ );
	f_open( &outputWrite, "output.dat", FA_CREATE_ALWAYS | FA_WRITE );
	f_open( &outputRead, "output.dat", FA_CREATE_ALWAYS | FA_READ );

	f_lseek( &inputWrite, 44100 * 2 );
	f_lseek( &inuptRead, 44100 * 2 );
	f_lseek( &outputWrite, 44100 * 2 );
	f_lseek( &outputRead, 44100 * 2 );

}

signed short process( signed short input, signed short delay, signed short echo )
{
	return input;
}

void push_output( signed short sample )
{
	buffer_write( &outputBuffer, sample, 1 );
	f_write( &outputWrite, sample, 2 );
}

void push_input( signed short sample )
{
	f_write( &inputWrite, sample, 2 );
}

signed short pop_output( void );
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
		f_read( &inputRead, &val, 2 );
		delayFilePos += 2;
		return val;
	}
	return 0;
}
signed short pop_echo( void );
{
	if ( processingCounter > delay )
	{
		signed short val;
		f_read( &outputRead, &val, 2 );
		echoFilePos += 2;
		return val;
	}
	return 0;
}

void do_work( void )
{
	// Check if we are not too far ahead
	if ( processingCounter < interruptCounter + MAX_LOOKAHEAD )
	{
		signed short combined = 0;

		// Load in sample from buttons to inputWrite
		for ( int i = 0; i < 2; ++i )
			if ( buttons[i]->playTime >= processingCounter )
			{
				signed short temp;
				f_read( buttons[i]->fp, temp, 2 );
				combined += temp;
			}
		push_input( combined );

		// Load sample into output
		signed short output = process( combined, pop_delay(), pop_echo() );
		output_push( output );

		processingCounter++;
	}
}

void load_set_one( unsigned long index )
{
	// Get button
	buttons[0] = &(cfg.buttons[index]);

	// Set beat interval play time
	buttons[0]->playTime = interruptCounter + ( interruptCounter % buttons[0]->interruptModulo );

	// Set file pointer
	buttons[0]->fp = &(buttonFiles[0]);
	f_open( buttons[0]->fp, files[index], FA_EXISTING | FA_READ );

}

void load_set_two( unsigned long index );
{
	// Get button
	buttons[1] = &(cfg.buttons[index]);

	// Set beat interval play time
	buttons[1]->playTime = interruptCounter + ( interruptCounter % buttons[1]->interruptModulo );

	// Set file pointer
	buttons[1]->fp = &(buttonFiles[0]);
	f_open( buttons[1]->fp, files[index], FA_EXISTING | FA_READ );
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
	delay = samples;
	f_lseek( &inputRead, offset );
}

