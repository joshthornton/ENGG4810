#ifndef _ENGG4810_CONFIG_H_
#define _ENGG4810_CONFIG_H_

#include "buffer.h"
#include "third_party/fatfs/src/ff.h"

#define NUM_BUTTONS 		(16)

#define MODE_LATCH 			(1)
#define MODE_HOLD 			(0)

#define ACTION_EFFECT_NONE	(0)
#define ACTION_EFFECT_ONE	(1)
#define ACTION_EFFECT_TWO	(2)

#define EFFECT_NONE			(0)
#define EFFECT_LOWPASS		(1 << 0)
#define EFFECT_HIGHPASS		(1 << 1)
#define EFFECT_BANDPASS		(1 << 2)
#define EFFECT_NOTCH		(1 << 3)
#define EFFECT_DELAY		(1 << 4)
#define EFFECT_ECHO			(1 << 5)
#define EFFECT_DECI_BIT		(1 << 6)
#define EFFECT_BITWISE		(1 << 7)

#define INTERRUPT_HERTZ		(44100)

#define COEFF_MASK  		(EFFECT_LOWPASS | EFFECT_HIGHPASS | EFFECT_BANDPASS | EFFECT_NOTCH)

typedef struct button {
	unsigned long index;				// the number button that it is
	unsigned long mode;					// Latch or Hold
	unsigned long action;				// FN Button Action
	unsigned long loopInterval;			// User specified loop interval
	unsigned long playTime;				// playing if playTime >= globalTimer
	unsigned long isLooped;
	unsigned long interruptModulo;		// Precalculated interrupt modulo
	FIL *fp;							// File pointer NOTE: not actually file buffer, just pointer (waste of memory)
} button;

typedef struct config {
	unsigned long effectOne;
	unsigned long effectTwo;
	unsigned long bpm;
	button buttons[NUM_BUTTONS];
} config;


// Global config variable
extern volatile config cfg;

void config_init( void );

#endif

