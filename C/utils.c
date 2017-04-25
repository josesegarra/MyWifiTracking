
#include "utils.h"
#include <errno.h>

#define MAX_LINE 1024
#define FMT_TIME "%02d-%02d-%02d %02d:%02d:%02d"																								// Format string for FULL date yyyy-MM-dd HH:mm:ss
#define FMT_DATE "%02d-%02d-%02d"																												// Format string for DATE yyyy-MM-dd 


char g1[1000];
static pthread_mutex_t lock;																															// Global lock for utils_log
char g2[1000];
int nlocks = 0;
char log_file_path[1024];																														// Path to log directory. 
char log_folder_path[1024];																														// Path to log directory. 
struct tm current_log;																															// This is a struct that contains the DATE for the current log file
	
/*
#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
	Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
#else
	usleep(milliseconds * 1000);
#endif
}
*/
void utils_lock(char *module,pthread_mutex_t* l)
{
	int ls= pthread_mutex_trylock(l);
	if (ls == 0)
	{
		nlocks++;
		return;
	}
	usleep(100*1000);													// Sleep thread for 100 milliseconds = 100 * 1000 microseconds
	ls = pthread_mutex_trylock(l);
	if (ls != 0) 	FINISH(" %s Could not get lock after timeout !!! ", module);
	nlocks++;
	//printf("    %s in 2nd attempt got lock %i\n", module, nlocks);
}

void utils_unlock(char* module,pthread_mutex_t* l)
{
	int ls = pthread_mutex_unlock(l);																													// Mutex of log file access
	if (ls != 0) FINISH(" %s Could not unlock !!! ", module);
	nlocks--;
}

char* utils_time_asStr(char* b,int wantHour,struct tm* t1)																						// Gets current time as a string
{
	time_t t0 = time(NULL);																														// Get current time
	localtime_r(&t0, t1);																														// As parts
	char* c=utils_write(b,0,wantHour?FMT_TIME:FMT_DATE, t1->tm_year + 1900, t1->tm_mon+1, t1->tm_mday,t1->tm_hour,t1->tm_min,t1->tm_sec);		// Write into B the time (don't add CR)
	return c;	
}

char* utils_catn(char* p1,char* p2,int size)																												// Concat and return end of concatenated string
{
    if (size <= 0) return p1;
    char* p3=strncat(p1,p2,size);
    return p3+strlen(p3);
}

char* utils_cat(char* p1, char* p2)																												// Concat and return end of concatenated string
{
    char* p3 = strcat(p1, p2);
    return p3 + strlen(p3);
}

int  utils_buffer(char* buf, int wantCr, char* f, va_list aptr)
{
	int ret;
	ret = vsnprintf(buf, MAX_LINE - 2, f, aptr);																								// Call sprintf in its sized+variadic list version	
	if (wantCr)	buf[ret++] = '\n';																												// If CR wanted then Insert it
	buf[ret] = '\0';																															// Terminate string
	return ret;
}

char* utils_write(char* buf,int wantCr,char *format, ...)
{
	va_list aptr;
	va_start(aptr, format);																														// Get variadic parameters as list
	int i=utils_buffer(buf,wantCr,format, aptr);																								// Print to buffer
	va_end(aptr);																																// Release list of parameteres	
	return buf+i;																																// Return message. Warning this buffer is not thread safe anymore
}


void utils_log(char *module,char *format, ...)
{
	if (module == NULL) module = "----\0";
	utils_lock(module, &lock);																													// Mutex of log file access, if mutex cannot be acquired then cancel login
	struct tm this_log;																															// This is a struct that contains the DATE for the current log file
	char b[MAX_LINE+300];																														// Buffer to receive string
	char* c=utils_time_asStr(b,1,&this_log);																									// Get time
	c=utils_cat(c,"  ");																														// Concat "  " to time
	va_list aptr;																																// Get parameters
	va_start(aptr, format);																														// Get variadic parameters as list
	utils_buffer(c,1,format, aptr);																												// Concat format + values
	va_end(aptr);																																// Release list of parameteres	
	printf("%-15s  %s",module,b);																												// Print to screen
	// Pending compare this_log with current_log and change log file name if needed
	utils_file_write(log_file_path,b);
	utils_unlock(module, &lock);																												// Finally unlock mutex
}


void utils_file_write(char* fileName,char* text)
{
	if ((!text) || (text[0]=='\0')) return;																										// left-to-right evaluation with short circuit http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1256.pdf in section 6.5.14 Logical OR 
	FILE *pFile=fopen(fileName, "a");																											// Open file for appending
	if (pFile) fprintf(pFile,text);	else printf("**** Could not access to log file %s \n",fileName); 											// Append to file or write an error
	fclose(pFile);																																// Close file
}

void utils_initlog()
{
	struct stat st = { 0 };																														// Needed to access file properties
	if (getcwd(log_folder_path, sizeof(log_folder_path)) == NULL) FINISH("Could not fetch current directory");										// Get current directory
	strcat(log_folder_path, "/logs/");																											// Add subfolder logs
	if (stat(log_folder_path, &st) == -1) mkdir(log_folder_path, 0777);																			// If logs subfolder does not exist create it
	if (stat(log_folder_path, &st) == -1) FINISH("Could not create log directory: %s", log_folder_path);										// If creation failed the CRASH
	strcpy(log_file_path, log_folder_path);
	char* c=utils_cat(log_file_path,"log_");
	c=utils_time_asStr(c,0,&current_log);
	strcat(log_file_path,".txt");
	utils_log(__FILE__,"Log file:                        %s", log_file_path);																							// Log...
}

void utils_init()
{
	pthread_mutexattr_t Attr;
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);

	if (pthread_mutex_init(&lock, NULL) != 0) FINISH("Could not init mutex lock in utils");									// This lock can be called from different threads so we use a recursive MUTEX			
	//if (pthread_mutex_init(&lock, &Attr) != 0) FINISH("Could not init mutex lock in utils");									// This lock can be called from different threads so we use a recursive MUTEX			
	utils_initlog();
}



p_stream_v utils_logs()
{
	
	utils_log("Display content of log folder: %s", log_folder_path);
	DIR *d = opendir(log_folder_path);																										// Open directory handler
	if (!d)																																	// If could not open
	{
		utils_log("utils_logs: Could not find folder: %s", log_folder_path);																// Log 
		return NULL;																														// And return null
	}
	p_stream_v st = stream_new();																												// Create a stream
	stream_write(st, "[");

	struct dirent *dir = readdir(d);																														// DirectoryEntity struct
	while (dir != NULL)																										// While reading in the directory
	{
		if (dir->d_type == DT_REG)	stream_write(st, "\"%s\" ", dir->d_name);																// If a regular item (ie: FILE, not ..,symlink,...) then write to stream
		dir = readdir(d);
		if (dir != NULL && dir->d_type == DT_REG) stream_write(st, ",");
	}
	stream_write(st, "]\n\n");
	closedir(d);
	stream_close(st);
	return st;
}


p_text  utils_ptext_new(char *info, int size)
{
    p_text t = malloc(sizeof(m_text));                                                                      // Create a new text node
    t->info = malloc(size + 2);                                                                             // Set size+2 for the new text node info
    memset(t->info, '\x00', size + 2);                                                                      // Reset new memeory to ZERO 
    strncpy(t->info, info, size);                                                                           // Copy size chars to new memory (we are sure that there is always a \0  terminator)
    t->next = NULL;                                                                                         // Next text is NULL
    return t;
}

p_text  utils_ptext_add(p_text root, char *info, int size)
{
    if (info == NULL || info[0] == '\0') return root;								                  // If receiving NULL or "" return ROOT
    if (root == NULL) return utils_ptext_new(info, size);                                                   // If list is EMPTY then return a new ROOT    
    p_text t = root;                                                                                        // Lets loop starting from ROOT
    while (1)
    {
        if (strcmp(t->info, info) == 0) return root;				                                      // If text already in list of text then return ROOT
        if (t->next == NULL) break; else t = t->next;                                                       // Keep looping until t->next is NULL    
    }
    t->next = utils_ptext_new(info, size);                                                                  // Create a new ptext item and add it to the end of the chain
    return root;                                                                                            // Return list root
}

char* utils_ptext_copy(p_text item, char* buffer, int bufsize)
{
    char*  b = buffer;
    if (buffer == NULL || bufsize<1) return;                                                                // Simple check
    memset(buffer, '\x00', bufsize);                                                                        // Reset buffer to ZERO 
    bufsize = bufsize - 4;                                                                                  // keep 4 \0 terminators
    while (item && bufsize>0)
    {
        int k = strlen(item->info);                                                                         // Get length of this node
        if (bufsize>(k + 3))                                                                                // If this node fits in buffer
        {
            strncpy(buffer, item->info, k);                                                                 // Copy the string
            buffer = buffer + k;                                                                            // Move forward the buffer
            if (item->next)                                                                                 // If there is other item
            {
                strncpy(buffer, " | ", 3);                                                                  // Copy separator
                buffer = buffer + 3;                                                                        // Move forward the buffer
            }
            bufsize = bufsize - k - 3;                                                                      // Decrease available space in buffer
        }
        else 
        {
            strncpy(buffer, "...", 3);
            break;
        }
        item = item->next;
    }
    return b;
}

void utils_trimcpy(char* d, char *s,int i)
{
    while (*s != '\0' && *s <= ' ') s++;                                                                    // Skip initial NON printable
    while (i && *s >= ' ')                                                                                  // While i>0 and *s>=' '
    {
        *d = *s;d++;s++;i--;                                                                                // Copy char, inc pointers and dec counter
    }
    *d = '\0';                                                                                              // Zero terminate    
}

/*
int mac_equals(unsigned char m0[6], unsigned char m1[6])
{
	return (m0[0] == m1[0] && m0[1] == m1[1] && m0[2] == m1[2] && m0[3] == m1[3] && m0[4] == m1[4] && m0[5] == m1[5]) ? 1 : 0;
}
*/
