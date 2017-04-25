

int x(int);

void start();
void init(char* iface);


typedef struct {
	char*			block;
	size_t			size;
} stream_v;

typedef stream_v* p_stream_v;

p_stream_v device_list();
void		   stream_free(p_stream_v	s);
