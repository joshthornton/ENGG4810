#ifndef _ENGG4810_POLL_H_
#define _ENGG4810_POLL_H_

#define LED_OFF		(0b00)
#define LED_RED		(0b01)
#define LED_GREEN	(0b10)
#define LED_BOTH	(0b11)

#define LED_SET(i,v) (ledState = (ledState&(~(0b11<<(i<<1)))) | (v<<(i<<1)))
#define LED_GET(i) ((ledState&(0b11<<(i<<1)))>>(i<<1))
#define BTN_GET(i) ((buttonState&(1<<i))>>i)
#define BTN_SET(i,v) (buttonState = (((buttonState&(~(1<<i)))>>i) | (v<<i)))

// Global effect variables
extern unsigned long effectValues[4];
extern unsigned long buttonState;
extern unsigned long ledState;

void poll_init( void );
void poll_interrupt( void );

#endif
