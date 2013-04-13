#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

#define TEST_BUFFER_SIZE (4)

int main( int argc, char *argv[] )
{

	buffer b;
	signed short temp[TEST_BUFFER_SIZE] = { 2, 3, 4, 1 };
	buffer_init( &b );

	unsigned long num;
	num = buffer_write( &b, temp + 1, 3 );
	buffer_print( &b );

	num = buffer_read( &b, temp + 1, 2 );
	buffer_print( &b );

	num = buffer_write( &b, temp, 3 );
	buffer_print( &b );

	num = buffer_read( &b, temp, 4 );
	buffer_print( &b );

	for ( int i = 0; i < TEST_BUFFER_SIZE; ++i )
		fprintf( stdout, "temp[%d]: %d\n", i, temp[i] );
	
	return EXIT_SUCCESS;
}
