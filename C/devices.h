

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"



/*
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
*/

#define KIND_ROOT            00
#define KIND_UNKNOWN         10
#define KIND_ACCESS_POINT    20
#define KIND_DEVICE          30




typedef struct i_device {
	unsigned char	mac[6];
	time_t			lastSeen;              // Time the device was last seen
     time_t			lastLog;               // Time the device was last logged
     long                times;                 // Number of times the device has been seen
     int				kind;                  // KIND_ACCES_POINT | KIND_DEVICE
	int				signal;                // Last value for signal
	int				channel;               // Last channel seen
     char*               vendor;                // Vendor name
     p_text			text;                  // Additional info

     struct i_device	*next;                 // Next device (or NULL if none)

} m_device;

typedef m_device* p_device;

void device_init();
void device_detected(unsigned char mac[6], int signal, int channel, char *info, int kind);

#endif
