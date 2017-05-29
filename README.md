# MyWifiTracking

Overview
========

There are situations where tracking the movement of people within the boundaries of a facility can be very convenient 
for a number of different reasons. Some examples:
* Security reasons: in a military installation monitoring that some visitors maintain themselves within the expected areas and do not wander (by mistake or purposely) into “unauthorized” zones.
* Personnel tracking: in a hospital, when a medical emergency arises a fast way of selecting, based on their location, which of the doctors on duty should be called (ie: do not bother calling the ones which are inside an operation room and choose the one closest to emergency location) can save lives.
* Facilities/Service Design: monitoring the usage of the cafeteria by staff at lunch time (ie: how many people use it, at what times, for how long, …) can diagnose its usage (ie: it is being overuse, underused,.. ) and define patrons of usage (ie: is the staff abusing its lunch time). These data can be correlated with other to define quality of service parameters (ie: how long does it take to heat the lunch )  and rationalize operational parameters (ie: reschedule lunch times, buy more microwaves,…)
All in all there can be hundreds of applications to this knowledge. Most of these applications are domain-specific, and as such they will have different “specific” needs. Some applications will require a precise identification of the person being tracked, others will care about the “swarm” behavior, some of them will need real-time feedback, others will need big bunches of data to identify “pattern” usage.

Nevertheless, as common needs of these applications, the tracking system should be:

* As un-intrusive as possible (the person being tracked shouldn’t be bothered)
* Easily reconfigured (adding/removing new areas to track), 
* Scalable (should work well for tracking a person or a hundred)
* Reliable (should give a low number of false positives or false negatives)
* Flexible (should work for identified or unidentified people)
* Open (should be easy to integrate with the applications that will use the tracking data)

And finally, although strictly they are not required system needs, there are competitive advantages that should be taken into account, the system should:

* Be cheap to implement: needed hardware
* Be easy to operate: needed personnel skills to set-up and configure 
* Be easy and cheap to maintain: detecting & replacing failing parts 
* Be easy to build: time, skills and resources needed 

Objective
=========

Many people today carry a mobile smartphone, and most of these smartphones include a Wi-Fi client. This project uses the behavior of these Wi-Fi clients to implement the tracking.

When a Wi-Fi  (phone, laptop ,..) is looking to connect to a known network, there are two approaches it can take. The first technique, which is used by laptops and most non-smartphone devices, involves scanning for Beacon Frames (packets broadcast by Wi-Fi routers in order to advertise their presence), waiting for a network that the client has previously connected to, and initiating a connection with it. The second technique, which is used primarily by smartphones, involves periodically broadcasting packets called Probe Requests, which contain the unique MAC address of the client and (sometimes) the name of a network to which it has previously connected. The advantage of the second technique is that by actively scanning for nearby routers, a phone can initiate a wireless connection faster than if it waits for the router to send out a Beacon Frame. While this certainly makes it more convenient to hop on a network, it also makes it possible to gather up this data and use it to track people. Usually the user is not aware of the signals its phone is sending, and so this tracking goes completely undetected.

The data that can be gathered data is fairly anonymous:
* Signal strength: how close is the phone to the detector
* MAC address: this is a device unique number (unless previously registered it cannot be related to the owner, but to the vendor of the device)

The gathered data (MAC address & signal strength) is not directly linkable to an identified user. This implies that for applications requiring identifying IDs a device registration stage is needed. Usually this would be a one-time step, that might even be skipped if the MAC address-user relation can be gathered from external sources of data (ie: inventory database, DHCP logs or pattern analysis).
Although false positives (detecting a device when it is not there) are not possible, a user can tamper the Wi-Fi signal that its device sends (MAC spoofing). This can lead to fake/wrong devices being detected. This is only of significance for applications requiring identified access. 
Another limitation is that not every person has a phone, not every phone is a smartphone, and not every smartphone has its Wi-Fi client enabled. Client Wi-Fi assurance is a must for the system to work, otherwise it can lead to false negatives (not detecting a device when it is there). 
The frequency at which Beacon Frames are sent depends on the specific device (model, OS, version,...). Most of the models have a 2 /3 cycle per minute. This gives a 20/30 seconds timeframe for detection. 

Implementation
==============

The project implements the described detector using a Raspberry Pi Model 2. This tracking system is made of:

* A libpnp library in C that after putting the Wifi dongle in promiscous mode will capture 802.11 packets, these packets are analyzed 
and grouped by MAC address, a simple vendor mapping is done based on the first three bytes of the mac address.
* A web server implemented in Go. This server hosts the previous library and exposes the list of detected MAC address, times detection, signal strength, announced networks and guessed vendor as a JSON REST API.
* A small Javascript application, available through the web server that creates a dashboard to display the previous JSON in a graphical manner.


