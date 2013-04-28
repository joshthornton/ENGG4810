#ifndef _ENGG4810_CONFIG_H_
#define _ENGG4810_CONFIG_H_

#include "buffer.h"

#define NUM_BUTTONS 		(16)

#define MODE_LATCH 			(0)
#define MODE_HOLD 			(1)

#define ACTION_EFFECT_NONE	(0)
#define ACTION_EFFECT_ONE	(1)
#define ACTION_EFFECT_TWO	(2)

#define EFFECT_NONE			(0)
#define EFFECT_LOWPASS		(1)
#define EFFECT_HIGHPASS		(2)
#define EFFECT_BANDPASS		(3)
#define EFFECT_NOTH			(4)
#define EFFECT_DELAY		(5)
#define EFFECT_ECHO			(6)
#define EFFECT_DECI_BIT		(7)
#define EFFECT_BITWISE		(8)

#define INTERRUPT_HERTZ		(44100)

typedef struct button {
	unsigned long mode;					// Latch or Hold
	unsigned long action;				// FN Button Action
	unsigned long loopInterval;			// User specified loop interval
	unsigned long isLooped;
	unsigned long playTime;				// playing if playTime <= globalTimer
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
extern config cfg;

void config_init( config *c );

#endif

