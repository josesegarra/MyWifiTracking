

#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#include "libs/pcap.h"				// struct pcap_file_header 
#include "osdep/osdep.h"			// struct wif	
#include "errno.h"					// errno

#define REFRESH_RATE 100000			// default delay in us between updates = 100 ms = 0.1 segundos, so this is 
#define KIND_DATA		 10
#define KIND_PROBE		 11
#define KIND_BEACON		 12
#define KIND_ACTION		 13


typedef enum {
	DT_NULL = 0,
	DT_WLANNG,
	DT_HOSTAP,
	DT_MADWIFI,
	DT_MADWIFING,
	DT_BCM43XX,
	DT_ORINOCO,
	DT_ZD1211RW,
	DT_ACX,
	DT_MAC80211_RT,
	DT_AT76USB,
	DT_IPW2200

} DRIVER_TYPE;

static const char * szaDriverTypes[] = {
	[DT_NULL] = "Unknown",
	[DT_WLANNG] = "Wlan-NG",
	[DT_HOSTAP] = "HostAP",
	[DT_MADWIFI] = "Madwifi",
	[DT_MADWIFING] = "Madwifi-NG",
	[DT_BCM43XX] = "BCM43xx",
	[DT_ORINOCO] = "Orinoco",
	[DT_ZD1211RW] = "ZD1211RW",
	[DT_ACX] = "ACX",
	[DT_MAC80211_RT] = "Mac80211-Radiotap",
	[DT_AT76USB] = "Atmel 76_usb",
	[DT_IPW2200] = "ipw2200"
};


struct priv_linux {
	int fd_in, arptype_in;
	int fd_out, arptype_out;
	int fd_main;
	int fd_rtc;

	DRIVER_TYPE drivertype; /* inited to DT_UNKNOWN on allocation by wi_alloc */

	void *f_sej_unused;						// NOT USED

	struct pcap_file_header pfh_in;

	int sysfs_inject;
	int channel;
	int freq;
	int rate;
	int tx_power;
	char *wlanctlng; /* XXX never set */
	char *iwpriv;
	char *iwconfig;
	char *ifconfig;
	char *wl;
	char *main_if;
	unsigned char pl_mac[6];
	int inject_wlanng;
};


void capture_init(char* iface);


#endif
