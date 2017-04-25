


#ifndef _UTILS_H_
#define _UTILS_H_

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
#include "stream.h"

#define TRUE    1
#define FALSE   0



#define FINISH(f_, ...) { printf( f_, ##__VA_ARGS__);printf("\n");exit(1); }					// Check this http://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
#define PRINT(f_, ...) { printf( f_, ##__VA_ARGS__);printf("\n"); }							// Check this http://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick


typedef struct i_text {
    char			*info;
    struct i_text	*next;
} m_text;
typedef m_text* p_text;


void			utils_init();															// Init utils			
void			utils_log(char *module, char *format, ...);									// Log
char*			utils_write(char* buf, int wantCr, char *format, ...);							// Write to buffer (insert CR or not)
char*			utils_cat(char* p1,char* p2);												// Concatenate and return last character of concatenated (which shiuld be /0)
char*			utils_catn(char* p1, char* p2, int size);									// Concat and return end of concatenated string

void			utils_trimcpy(char* d, char *s, int i);                                              // Trim copies a text    

char*			utils_time_asStr(char* b, int wantHour, struct tm* t1);						// Gets current time as a string
void			utils_file_write(char* fileName,char* text);									// Writes to file
p_stream_v		utils_logs();

p_text         utils_ptext_add(p_text root, char *info, int size);                                  // List of text !!
char*          utils_ptext_copy(p_text list, char* buffer, int bufsize);                            // Copies a list of text into a buffer



#endif
