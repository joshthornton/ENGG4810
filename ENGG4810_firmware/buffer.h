#ifndef _ENGG4810_BUFFER_H_
#define _ENGG4810_BUFFER_H_

#define BUFFER_SIZE (2048)

typedef struct buffer {
	signed short b[BUFFER_SIZE];
	unsigned long start;
	unsigned long length;
	unsigned long size;
} buffer;

#endif
