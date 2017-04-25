
#ifndef _STREAM_H_
#define _STREAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>


typedef struct {
	char*			block;
	size_t			size;
	FILE*			stream;
} stream_v;

typedef stream_v* p_stream_v;

p_stream_v	stream_new();
void				stream_close(p_stream_v	s);
void				stream_free(p_stream_v	s);
void				stream_write(p_stream_v	s, const char * format, ...);
void       	stream_copy(p_stream_v, char *data, int size);

#endif
