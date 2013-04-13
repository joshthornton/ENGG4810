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

typedef struct button {

	// Button config
	unsigned long mode;
	unsigned long action;
	unsigned long loopInterval;

	// Button buffer
	buffer buf;

	// Button I/O
	unsigned long fileIndex;
} button;

typedef struct config {
	unsigned long effectOne;
	unsigned long effectTwo;
	unsigned long bpm;
	button buttons[NUM_BUTTONS];
} config;


// Global config variable
config cfg;

#endif
