#ifndef _ENGG4810_LOAD_H_
#define _ENGG4810_LOAD_H_

#include "config.h"

void playback_interrupt( void );

void load_init( void );
void load_set_one( unsigned long index );
void load_set_two( unsigned long index );
void load_set_delay( unsigned long samples );
void load_set_echo( unsigned long samples );

void do_work( void );

#endif
