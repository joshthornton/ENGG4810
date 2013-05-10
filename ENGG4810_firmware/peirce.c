#include <avr/io.h>

int main ( void )
{

	// setup
	DDRC = 0x00; // input
	DDRF = 0xFF; // output

	// forever
	while( 1 )
	{
		// lock
		PORTF = 0x01; // Locked

		// wait until button pressed
		while( !( PINC & 0x01 ) );

		// get first value
		int first = PINC >> 4; // shift right by four places

		// wait until button pressed again
		while( !( PINC & 0x01 ) );

		// get second value
		int second = PINC >> 4; // shift right by four places

		// check values -> unlock
		if ( first == 0 && second == 9 ) {
			PORTF = 0x02;
		}

		// wait for button press before relocking
		while( !( PINC & 0x01 ) );
	}

}
