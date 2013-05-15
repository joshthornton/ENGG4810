#ifndef _ENGG4810_LOAD_H_
#define _ENGG4810_LOAD_H_

#include "config.h"

#define STOP_PLAYING (0xFFFFFFFF) //max unsigned long

void playback_interrupt( void );

void load_init( void );
void load_set_one( unsigned long index );
void load_set_two( unsigned long index );
void load_set_delay( unsigned long size, float amount );
void load_set_echo( unsigned long size, float amount );

void do_work( void );
void test (void);

extern unsigned long interruptCounter;
void load_generate_coeffs(int effect, int Fc, float Q);

#endif
