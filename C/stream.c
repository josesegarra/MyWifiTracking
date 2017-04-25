
#include "stream.h"



p_stream_v		stream_new()
{
	p_stream_v s = malloc(sizeof(stream_v));
	s->block = NULL;
	s->size = 0;
	s->stream = open_memstream(&(s->block), &(s->size));																								// Open a memory stream
	//printf("****** Opened stream with HANDLE %i \n",(int)s->stream);
	return s;
}

void		stream_free(p_stream_v	s)
{
	if (s->stream != 0) fclose(s->stream);
	if (s->block != NULL) free(s->block);
	free(s);
}

void		stream_close(p_stream_v	s)
{
	fclose(s->stream);
	s->stream = 0;
}


void		stream_write(p_stream_v	s, const char * format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(s->stream, format,args);
	va_end(args);
	fflush(s->stream);
}																// Write to stream

void       stream_copy(p_stream_v s, char *data, int size)
{
	fwrite(data, size, 1, s->stream); 
	fflush(s->stream);
}