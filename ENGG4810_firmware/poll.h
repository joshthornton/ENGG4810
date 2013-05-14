#ifndef _ENGG4810_POLL_H_
#define _ENGG4810_POLL_H_

#define LED_OFF		(0x00)
#define LED_RED		(0x01)
#define LED_GREEN	(0x02)
#define LED_BOTH	(0x03)

// Global state variables
extern unsigned long buttonState[4][4];
extern unsigned long ledState[4][4];

void poll_init( void );
void poll_interrupt( void );

#endif
