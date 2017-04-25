


#include "capture.h"
#include "utils.h"

#include <sys/ioctl.h>
#include <linux/if.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

pthread_t threadId;
char*		s_iface;										// interface name 
struct wif	*wi;											// wifi driver 
int fd_raw,channel;

static char* module = __FILE__; // "devices.c";



int handle_data(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	unsigned char ad1[6];
	unsigned char ad2[6];
	unsigned char ad3[6];

	memcpy(ad1, h80211 + 4, 6);                                    // Get Access Point MAC address    
	memcpy(ad2, h80211 + 10, 6);                                    // Get Access Point MAC address    
	memcpy(ad3, h80211 + 16, 6);                                    // Get Access Point MAC address    

	// int dir = h80211[1] & 3;
	// Dir = 0 communication inside the SSD. AD1= Destination AD2= Source	AD3 = BSSID
	// Dir = 1 coming from outside SSD.      AD1= Destination AD2= BSSID	AD3 = Source
	// Dir = 2 going outside SSD				AD1 = BSSID      AD2= Source  AD3 = Destination
	// Dir = 3 Pass throught					AD1 = Receiving Adress Ad2= Transmitting Ad3 = Destinatin Ad4 = Source

     switch (h80211[1] & 3)
     {
        //case 0: device_detected(ad2, ri->ri_power, ri->ri_channel, "traffic internal\0", KIND_DATA); break;
        //case 1: device_detected(ad3, ri->ri_power, ri->ri_channel, "traffic incoming\0", KIND_DATA); break;
        //case 2: device_detected(ad2, ri->ri_power, ri->ri_channel, "traffic outgoing\0", KIND_DATA); break;
        //case 3: device_detected(ad2, ri->ri_power, ri->ri_channel, "traffic pass throught\0", KIND_DATA); break;

        case 0: device_detected(ad2, ri->ri_power, ri->ri_channel, NULL, KIND_DATA); break;
        case 1: device_detected(ad3, ri->ri_power, ri->ri_channel, NULL, KIND_DATA); break;
        case 2: device_detected(ad2, ri->ri_power, ri->ri_channel, NULL, KIND_DATA); break;
        case 3: device_detected(ad2, ri->ri_power, ri->ri_channel, NULL, KIND_DATA); break;

     }
	return 0;
}

int handle_noinfo(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	// RTS frame:  Duration field contain value in µs of time need to transmit data or management + CTS + ACK + SIFS interval
	return 0;
}



int handle_qsdata(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	unsigned char ad1[6];
	unsigned char ad2[6];
	unsigned char ad3[6];
	unsigned char ad4[6];

	memcpy(ad1, h80211 + 4, 6);                                    // Get Access Point MAC address    
	memcpy(ad2, h80211 + 10, 6);                                    // Get Access Point MAC address    
	memcpy(ad3, h80211 + 16, 6);                                    // Get Access Point MAC address    
	memcpy(ad4, h80211 + 24, 6);                                    // Get Access Point MAC address    

	printf("Received QS data frame!! \n");

	//printf("QS Data Flow %i Ad1 ", h80211[1] & 3);//print_mac(ad1);
     //printf("Ad2 "); print_mac(ad2);
     //printf("Ad3 "); print_mac(ad3);
     //printf("Ad4 "); print_mac(ad4);
     //printf("\n");
	return 0;
}


//http://www.rfwireless-world.com/Terminology/WLAN-probe-request-and-response-frame.html
//https://www.safaribooksonline.com/library/view/80211-wireless-networks/0596100523/ch04.html

int handle_probereq(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	unsigned char mac[6];
	unsigned char essid[100];
	unsigned char *p = h80211 + 24;                                                                                // Get to PROBE data
     if (p + 2 + p[1]> h80211 + caplen) return -1;								                          // If network length bigger than packet size
     memset(essid, '\x00', 100);                                                                                    // Set buffer to ZERO
	memcpy(mac, h80211 + 10, 6);																      // Copy MAC address to buffer. All other addresses are FF
	if ((p < h80211 + caplen))                                                                                     // If there is PROBE data 
     {
         if (p[0] == 0x00 && p[1] > 0 && p[2] != '\0' && (p[1] > 1 || p[2] != ' '))  memcpy(essid, p + 2, p[1]);    // If PROBE data is a valid string copy it
	}
     device_detected(mac, ri->ri_power, ri->ri_channel, essid, KIND_PROBE);                                         // Notify device detection
     return 0;
}


int handle_beacon(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	// MAC header is 24 bytes long. In BEACON frame BODY:
	//  8 bytes of time span
	//  2 bytes beacon interval
	//  2 bytes capability info
	//  So SSID variable: starts at 12 of BEACON FRAME BODY and AT 36 of MAC HEADER
	unsigned char essid[100];
	memset(essid, '\x00', 100);
	unsigned char mac[6];
	unsigned char *p = h80211 + 36;												                     // Skip hdr + fixed params, and go to BEACON data
	if (p + 2 + p[1]> h80211 + caplen) return -1;								                          // If network length bigger than packet size
	memcpy(mac, h80211 + 16, 6);												                          // Copy MAC address to buffer. 
	memcpy(essid, p + 2, (p[1]>80 ? 80 : p[1]));								                               // Copy ESSID name
    device_detected(mac, ri->ri_power, ri->ri_channel, essid, KIND_BEACON);                                        // Notify device detection
	return 0;
}

// https://mrncciew.com/2014/10/09/802-11-mgmt-action-frames/			DA - SA - BSS ID
// https://mrncciew.com/2014/11/03/cwap-data-frame-address-fields/
int handle_action(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	return;																				// Let´s ignore action frames
	unsigned char ad1[6];
	unsigned char ad2[6];
	unsigned char ad3[6];

	memcpy(ad1, h80211 + 4, 6);										// Destination
	memcpy(ad2, h80211 + 10, 6);                                    // Source 
	memcpy(ad3, h80211 + 16, 6);                                    // Basic service ID

	device_detected(ad2, ri->ri_power, ri->ri_channel, NULL, KIND_ACTION); 
	return 0;
}

int capture_packet(unsigned char *h80211, int caplen, struct rx_info *ri)
{
	int n, seq, i, haveStation = 1;
	unsigned char bssid[6];
	unsigned char stmac[6];
	unsigned char namac[6];
	unsigned char essid[50];

	unsigned char *p;

	memset(stmac, '\x00', 6);
	memset(namac, '\x00', 6);
	memset(essid, '\x00', 50);

	// El primer BYTE contiene VERSION-TYPE & SUBTYPE: 
	// http://www.studioreti.it/slide/802-11-Frame_E_C.pdf
	// HERE ARE MORE: http://www.rfwireless-world.com/Terminology/WLAN-MAC-frame-types.html 
	// Subtype [7..4]   Type [3..2]   Version [1..0] 
	//         0000           00             00   -- Management: Association request           $00
	//         0001           00             00   -- Management: Association response          $10
	//         0010           00             00   -- Management: Reassociation request         $20
	//         0011           00             00   -- Management: Reassociation response        $30
	//         0100           00             00   -- Management: Probe request                 $40
	//         0101           00             00   -- Management: Probe response                $50
	//         1000           00             00   -- Management: Beacon:                       $80
	//         1001           00             00   -- Management: ATIM                          $90
	//         1010           00             00   -- Management: Disassociation                $A0
	//         1011           00             00   -- Management: Authentication                $B0
	//         1100           00             00   -- Management: Deauthentication              $C0
	//         1101           00             00   -- Management: Action						   $D0


	//         1010           01             00   -- Control: PS Poll                          $A4
	//         1011           01             00   -- Control: RTS                              $B4
	//         1100           01             00   -- Control: CTS                              $C4
	//         1101           01             00   -- Control: ACK                              $D4
	//         1110           01             00   -- Control: CFE                              $E4
	//         1111           01             00   -- Control: CFE+ACK                          $F4

	//         0000           10             00   -- Data: Data                                $08 
	//         0001           10             00   -- Data: Data + CF-Ack                       $18
	//         0010           10             00   -- Data: Data + CF-Poll                      $28
	//         0011           10             00   -- Data: Data + CF-Ack + CF-Poll             $38
	//         0100           10             00   -- Data: Null                                $48
	//         0101           10             00   -- Data: CF-Ack (no data)                    $58
	//         0110           10             00   -- Data: CF-Poll (no data)                   $68
	//         0111           10             00   -- Data: CF-Ack + CF-Poll (no data)          $78

	//         1000           10             00   -- QS Data: Data                             $88
	//         1001           10             00   -- QS Data: Data + CF - Ack
	//         1010           10             00   -- QS Data: 	QoS Data + CF - Poll
	//         1011           10            00   -- QS Data: QoS Data + CF - Ack + CF - Poll
	//         1100           10            00   -- QS Data: QoS Null(no data)
	//         1110           10            00   -- QS Data: QoS CF - Poll(No data)
	//         1111           10            00   -- QS Data: QoS CF - Ack + CF - Poll(No Data)


	if (h80211[0] == 0x80) return handle_beacon(h80211, caplen, ri);
	if (h80211[0] == 0x40) return handle_probereq(h80211, caplen, ri);
	if (h80211[0] == 0x08) return handle_data(h80211, caplen, ri);
	if (h80211[0] == 0x88) return handle_qsdata(h80211, caplen, ri);
	if (h80211[0] == 0xD0) return handle_action(h80211, caplen, ri);
	if (h80211[0] == 0xB4 || h80211[0] == 0x48 || h80211[0] == 0xD4) return handle_noinfo(h80211, caplen, ri);

	// Do not STOP on FINISH("Unsupported FRAME type %02X \n", h80211[0]);
     printf("Unsupported FRAME type %02X \n", h80211[0]);
     return 0;
}


void capture_main(void *arg)																										// Main server loop, this is executed in a dedicated thread
{
	int caplen = 0;
	struct timeval     tv0;
	unsigned char      buffer[4096];
	unsigned char      *h80211= buffer;
	fd_set             rfds;
	struct rx_info     ri;

	while (1){																															// Main app loop
		FD_ZERO(&rfds);																													// Init IO async structure
		FD_SET(fd_raw, &rfds);																											// Init IO for interface handler
		tv0.tv_sec = 0;
		tv0.tv_usec = REFRESH_RATE;
		if (select(fd_raw + 1, &rfds, NULL, NULL, &tv0) < 0)																			// Check http://linux.die.net/man/2/select http://www.tutorialspoint.com/unix_system_calls/_newselect.htm 
		{
			if (errno == EINTR) continue;																								// If select failed becouse of EINTR signal, then continue
			FINISH("ERROR [%i] select failed in: %s\n%s", errno, s_iface, strerror(errno));												// else if error was for other reason fail
		}
		if (FD_ISSET(fd_raw, &rfds))																									// If any packet arrived
		{
			memset(buffer, 0, sizeof(buffer));																							// Set buffer =0
			if ((caplen = wi_read(wi, h80211, sizeof(buffer), &ri)) == -1) FINISH("Could not read package. %i\n", 1);					// If cannot read data
			capture_packet(h80211, caplen, &ri);
		}
	}
}


/*
http://stackoverflow.com/questions/1325378/packet-socket-in-promiscuous-mode-only-receiving-local-traffic
https://bytes.com/topic/c/answers/826456-setting-nic-promiscuous-mode
int promisc(){
    int fd;
    struct ifreq eth;

    fd = socket(AF_INET, SOCK_PACKET, htons(0x800));                    // Init a socket file descriptor
    strcpy(eth.ifr_name, s_iface);                                      // Init a ifreq_structure, and set it to the name of the interface
    ioctl(fd, SIOCGIFFLAGS, &eth);                                      // Using IOCT get the FLAGS of the device
    if (!(eth.ifr_flags && IFF_PROMISC))                                // If PROMISCOUS flag is not on
    {
        eth.ifr_flags |= IFF_PROMISC;                                   // Add it to available flags
    //    ioctl(fd, SIOCSIFFLAGS, &eth);                                  // And set it
    }
    ioctl(fd, SIOCGIFFLAGS, &eth);                                      // Check if PROMISCOUS mode is on
    utils_log(module, "PROMISCUOUS mode is %s", (eth.ifr_flags && IFF_PROMISC) ? "ON" : "OFF");
}
*/

/*How to enter into monitor mode
sudo ifconfig wlan0 down
sudo iwconfig wlan0 mode monitor
sudo ifconfig wlan0 up


IW_MODE_MONITOR
https://github.com/vy/wapi/blob/master/src/wireless.c


int
wapi_get_mode(int sock, const char *ifname, wapi_mode_t *mode)
{
struct iwreq wrq;
int ret;

WAPI_VALIDATE_PTR(mode);

strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
if ((ret = ioctl(sock, SIOCGIWMODE, &wrq)) >= 0)
ret = wapi_parse_mode(wrq.u.mode, mode);
else WAPI_IOCTL_STRERROR(SIOCGIWMODE);

return ret;
}


int
wapi_set_mode(int sock, const char *ifname, wapi_mode_t mode)
{
struct iwreq wrq;
int ret;

wrq.u.mode = mode;

strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
ret = ioctl(sock, SIOCSIWMODE, &wrq);
if (ret < 0) WAPI_IOCTL_STRERROR(SIOCSIWMODE);

return ret;
}

int
wapi_get_ifup(int sock, const char *ifname, int *is_up)
{
struct ifreq ifr;
int ret;

WAPI_VALIDATE_PTR(is_up);

strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
if ((ret = ioctl(sock, SIOCGIFFLAGS, &ifr)) >= 0)
*is_up = (ifr.ifr_flags & IFF_UP) == IFF_UP;
else WAPI_IOCTL_STRERROR(SIOCGIFFLAGS);

return ret;
}


int
wapi_set_ifup(int sock, const char *ifname)
{
struct ifreq ifr;
int ret;

strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
if ((ret = ioctl(sock, SIOCGIFFLAGS, &ifr)) >= 0)
{
ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
ret = ioctl(sock, SIOCSIFFLAGS, &ifr);
}
else WAPI_IOCTL_STRERROR(SIOCGIFFLAGS);

return ret;
}

*/
void capture_init(char* iface)
{
	utils_log(module, "Capturing on:                    %s", iface);
	s_iface = iface;																												// Get the interface name											
    wi = wi_open(s_iface);																											// Open interface
    if (!wi) FINISH("Could not wi_open: %s\n", s_iface);																			// Need at least ONE interface
	fd_raw = wi_fd(wi);																												// Get interface handler
	channel = wi_get_channel(wi);																										// Get channel
	utils_log(module, "Opened interface:                %s [%i] using channel %i", wi->wi_interface, fd_raw, channel);											// Display channel
	struct priv_linux *dev = wi_priv(wi);
	utils_log(module, "                                 driver type  %i - %s", dev->drivertype, szaDriverTypes[dev->drivertype]);
	if (wi_get_monitor(wi)) FINISH("Could not open in monitor mode: %s\n", s_iface);

	int err = pthread_create(&threadId, NULL, &capture_main, NULL);
	if (err != 0) utils_log(module, "\ncan't create thread :[%s]", strerror(err));

}


