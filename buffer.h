#ifndef _ENGG4810_BUFFER_H_
#define _ENGG4810_BUFFER_H_

#define BUFFER_SIZE (4)

typedef struct buffer {
	signed short b[BUFFER_SIZE];
	unsigned long start;
	unsigned long length;
} buffer;

void buffer_init( buffer* b );
unsigned long buffer_read( buffer* b, signed short dst[], unsigned long maxlen );
unsigned long buffer_write( buffer* b, signed short src[], unsigned long maxlen );
void buffer_print( buffer* b );
inline unsigned long buffer_free_space( buffer* b )
{
	return BUFFER_SIZE - b->length;
}

#endif
