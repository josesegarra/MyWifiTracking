
#include <time.h>
#include "devices.h"
#include "capture.h"

// MACRO to check if two MAC addresses are the SAME
#define SAME_MAC(m0,m1)  ((m0[0] == m1[0] && m0[1] == m1[1] && m0[2] == m1[2] && m0[3] == m1[3] && m0[4] == m1[4] && m0[5] == m1[5]) ? 1 : 0)

// Time format
#define FMT_TIME "%02d-%02d-%02d %02d:%02d:%02d"																								// Format string for FULL date yyyy-MM-dd HH:mm:ss

// Minumum interval for loging upon message type
#define LOG_MSG_SECONDS_BEACON		10
#define LOG_MSG_SECONDS_PROBE		10
#define LOG_MSG_SECONDS_DATA		5
#define LOG_MSG_SECONDS_ACTION		10

static char* module = __FILE__;                                                                                 // Id of this module used in logging "devices.c";
p_device	devices= NULL;													                                                              // Linked list of devices
static pthread_mutex_t lock;														                                                        // Lock used to protect the previous linked list


// device_kind: Returns a device kind string from a message kind
char* device_kind(int i)
{
    char* kstr                 = "UNDEFINED";                                                                   // Default for device is UNDEFINED
    if (i == KIND_ACCESS_POINT) kstr = "Access Point";                                                         // If device is ACCESS POINT then return this STRING
    if (i == KIND_DEVICE)       kstr = "Device";                                                               // If device is DEVICE       then return this STRING
    return kstr;
}

// device_new: Creates a p_device with given KIND & MAC
p_device device_new(unsigned char mac[6], int kind)
{
    p_device device = (p_device)malloc(sizeof(m_device));                                                       // Create the device
    memset(device, 0, sizeof(m_device));                                                                        // Initalize ALL fields to ZERO or NULL
    memcpy(&device->mac, mac, 6);                                                                               // Copy MAC addres
    device->kind = kind;                                                                                        // Set KIND
    device->vendor = vendor_name(mac);                                                                          // Get device vendor name
    return device;                                                                                              // Return the created device
}

// devices_init: Initializes support for handling devices data
void device_init()
{
	if (pthread_mutex_init(&lock, NULL) != 0) FINISH("******* Mutex [lock] init failed");                      // Init the device list
     devices = device_new("\0\0\0\0\0\0", KIND_ROOT);                                                           // Create the ROOT device as list start (to avoid NULL checking on each list operation)
	utils_log(module,"Init root device:                %02X:%02X:%02X:%02X:%02X:%02X",                         // Log creation of ROOT device
         devices->mac[0], devices->mac[1], devices->mac[2], devices->mac[3],
         devices->mac[4], devices->mac[5]);
}

// device_find: Finds a device in the devices chain
p_device device_find(unsigned char mac[6])
{
    p_device w = devices,n = NULL;                                                                             // w: device pivot | n: device found
	while (w->next && n==NULL) if (SAME_MAC(w->next->mac, mac)) n = w->next; else w = w->next;                 // While (pivot is valid & NOT device found) DO if (PIVOT.MAC is wanted) n=pivot else move pivot
     if (n) return n;                                                                                           // If device found return it
     return (w->next = device_new(mac, KIND_UNKNOWN));                                                          // Create device, set it as last pivot and return it
}

// device_updatekind: Updates a device kind based on the kind of the received message
void device_updatekind(p_device device, int kind)
{
    switch (kind)
    {
        case KIND_BEACON:                                                                                       // If received a BEACON
            if (device->kind != KIND_ACCESS_POINT) device->kind = KIND_ACCESS_POINT;                            //      then it is an ACCESS_POINT
            break;
		case KIND_ACTION:
		case KIND_PROBE:                                                                                        // IF received a DATA  message
        case KIND_DATA:                                                                                         // IF received a PROBE message
            if (device->kind == KIND_UNKNOWN) device->kind = KIND_DEVICE;                                       //      If still unkown this is a DEVICE
            break;
    }
}

// device_update: Update devices data
void device_update(p_device device, time_t ctime, int signal, int channel, char* info)
{
    device->times++;                                                                                            // Increase the number of times the devices has been detected
    device->lastSeen = ctime;                                                                                   // Update device last time seen
    device->signal = signal;                                                                                    // Update SIGNAL value for device
    device->channel = channel;                                                                                  // Update CHANNEL value for device
    if (info != NULL) device->text = utils_ptext_add(device->text, info, 48);                                   // If there is INFO add it to devices list
}

// device_shouldlog: Returns 1 if the detected device should be logged and 0 if not
int device_shouldlog(p_device device, int kind,time_t ctime)
{
    if (device->lastLog == 0) return 1;                                                                         // If device has never been logged then log it
    int k = 1;
    long nt = ctime - device->lastLog;
    switch (kind)
    {
        case KIND_BEACON:   if (nt<LOG_MSG_SECONDS_BEACON) k = 0; break;
        case KIND_DATA:     if (nt<LOG_MSG_SECONDS_DATA) k = 0; break;
        case KIND_PROBE:    if (nt<LOG_MSG_SECONDS_PROBE) k = 0; break;
		case KIND_ACTION:	if (nt < LOG_MSG_SECONDS_ACTION) k = 0; break;

        default: FINISH ("IN device SHOULD LOG bad message: %i \n", kind);
    }
    return k;                                                                                                   // ELSE do NOT log
}

// devices_add: Updates a device data
void device_detected(unsigned char mac[6], int signal, int channel, char *info, int kind)
{
     pthread_mutex_lock(&lock);                                                                                 // Only one thread can UPDATE device data
     //if (info != NULL) printf("++++++ %02X:%02X:%02X:%02X:%02X:%02X  %s\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], info);
     p_device device = device_find(mac);                                                                        // Find the device in the devices list (if not present then create it)
     if (!device) FINISH("******* Could not add device");                                                       // This could only happen if out of memory or if devices list was corrupt
     device_updatekind(device, kind);                                                                           // Update device kind
     time_t ctime = time(NULL);                                                                                 // Get current time
     device_update(device, ctime, signal, channel, info);                                                       // Update devices data
     if (device_shouldlog(device, kind,ctime))                                                                       // If device detection should be logged
     {
         device->lastLog = ctime;                                                                               // Update logging time
         char buffer[200];                                                                                      // Stack buffer for device->text
         utils_log(module, "> %-15s %-30s %02X:%02X:%02X:%02X:%02X:%02X %c [%4i] %2i  | %4i  %s",               // Log device message
             device_kind(device->kind),                                                                         // Print device KIND
             device->vendor,                                                                                    // Print device vendor
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],                                                    // Print device MAC address
             (device->times == 1 ? '*' : ' '),                                                                  // Print '*' or ' ' whether is the first device detection
             signal, channel, device->times,                                                                    // Print SIGNAL strength, CHANNEL & TIMES detected
             utils_ptext_copy(device->text, buffer, 200));                                                      // Print device TEXT (if any)
     }
     pthread_mutex_unlock(&lock);
}



char* device_time_str(char* b, time_t lastSeen)
{
    struct tm t1;																						// Gets current time as a string
    localtime_r(&lastSeen, &t1);																														// As parts
    sprintf(b, FMT_TIME,t1.tm_year + 1900, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, t1.tm_min, t1.tm_sec);		// Write into B the time (don't add CR)
    return b;
}

void device_write(p_device w, p_stream_v st, char c)
{
	char m1[50];
	char m2[200];

	stream_write(st,
		" %c { \"last\":\"%s\" , \"mac\": \"%02X:%02X:%02X:%02X:%02X:%02X\", \"signal\": %i , \"kind\":\"%s\",\"vendor\":\"%s\" , \"info\":\"%s\" }\n",
		c,
		device_time_str(m1, w->lastSeen),
		w->mac[0], w->mac[1], w->mac[2], w->mac[3], w->mac[4], w->mac[5],
		w->signal, device_kind(w->kind),
		w->vendor,
		utils_ptext_copy(w->text, m2, sizeof(m2)));
}

p_stream_v device_list()
{
	//utils_log(module, "Preparing list of devices");
	pthread_mutex_lock(&lock);                                                                        // Only one thread can UPDATE device data

	p_stream_v st = stream_new();																												              // Create a stream
	stream_write(st, "[\n");

	p_device w = devices;                                                                             // w: device pivot | n: device found
	char c = ' ';
	while (w->next)
	{
		w = w->next;
		device_write(w, st, c);
		c = ',';
	}
	stream_write(st, "]\n\n\0");
	pthread_mutex_unlock(&lock);
	stream_close(st);
	return st;
}
