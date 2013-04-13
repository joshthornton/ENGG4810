#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "config.h"

int main( int argc, char *argv[] )
{

	printf( "unsigned long: %ld\n", sizeof( unsigned long ) );
	printf( "buffer: %ld\n", sizeof( buffer ) );	
	printf( "button: %ld\n", sizeof( button ) );	
	printf( "config: %ld\n", sizeof( config ) );	
	return EXIT_SUCCESS;
}
