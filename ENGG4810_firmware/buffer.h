#ifndef _ENGG4810_BUFFER_H_
#define _ENGG4810_BUFFER_H_

#define BUFFER_SIZE (4096)

typedef struct buffer {
	signed short b[BUFFER_SIZE];
	unsigned long start;
	unsigned long length;
	unsigned long size;
} buffer;

// Set the circular buffer size.
// Effectively delay/echo time
inline void buffer_set_size( buffer * b, unsigned long size )
{
	b->size = ( size < BUFFER_SIZE ) ? size : BUFFER_SIZE;
}

// Always succeeds
// will overwrite value b->size values ago
inline void buffer_push( buffer *b, signed short val )
{
	b->b[b->start] = val;
	b->start = ( b->start + 1 ) % b->size;
}

// Always succeeds
// will return the oldest value within b->size
inline signed short buffer_pop( buffer *b )
{
	return b->b[ ( b->start + 1 ) % b->size ];
}

inline void buffer_init( buffer *b )
{
	unsigned long i;
	b->start = 0;
	b->length = 0;
	b->size = 0;

	// Initialize to zero
	for ( i = 0; i < BUFFER_SIZE; ++i )
		b->b[i] = 0;
}

#endif
