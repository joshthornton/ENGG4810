#include "buffer.h"
#include "misc.h"
#include <string.h>
#include <stdio.h>

  void* mymemcpy(void* dest, const void* src, size_t count) {
        char* dst8 = (char*)dest;
        char* src8 = (char*)src;

        while (count--) {
            *dst8++ = *src8++;
        }
        return dest;
    }

// Force creation of external symbol
extern inline unsigned long buffer_free_space( buffer* b );

void buffer_init( buffer* b )
{
	b->start = 0;
	b->length = 0;
}

unsigned long buffer_read( buffer* b, signed short dst[], unsigned long maxlen )
{
	maxlen = MIN( b->length, maxlen ); 
	unsigned long middle = ( b->start + maxlen < BUFFER_SIZE )? b->start + maxlen : BUFFER_SIZE; 

	// Copy from start to middle
	mymemcpy( dst, b->b + b->start, (int)( ( middle - b->start ) * sizeof( signed short ) ) );

	// Copy from middle up to maxlen
	mymemcpy( dst + middle - b->start, b->b + middle % BUFFER_SIZE, (int)( ( maxlen - ( middle - b->start ) ) * sizeof( signed short ) ) );

	// Update pointer
	b->start = ( b->start + maxlen ) % BUFFER_SIZE; 
	b->length -= maxlen;

	return maxlen;
}

unsigned long buffer_write( buffer* b, signed short src[], unsigned long maxlen )
{
	maxlen = MIN( buffer_free_space( b ), maxlen ); 
	unsigned long middle = ( ( b->start + b->length % BUFFER_SIZE ) + maxlen < BUFFER_SIZE )? ( b->start + b->length % BUFFER_SIZE ) + maxlen : BUFFER_SIZE;

	// Copy from end to middle
	mymemcpy( b->b + ( b->start + b->length % BUFFER_SIZE ), src, (int)( ( middle - ( b->start + b->length % BUFFER_SIZE ) ) * sizeof( signed short ) ) );

	// Copy from middle up to maxlen
	mymemcpy( b->b + middle % BUFFER_SIZE , src + middle - ( b->start + b->length % BUFFER_SIZE ), (int)( ( maxlen - ( middle - ( b->start + b->length % BUFFER_SIZE ) ) ) * sizeof( signed short ) ) );

	// Update pointer
	b->length += maxlen;

	return maxlen;
}
