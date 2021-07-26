# RF24Ethernet Overview

## Overview
The RF24Ethernet library was initially designed as an experiment and potential testing tool for [RF24Network](http://nRF24.github.io/RF24Network), an OSI Layer 3 network driver, allowing a Raspberry Pi to
act as a TCP/IP gateway or host for connected sensor nodes. An Arduino can interface with any Linux machine or SLIP capable device supporting USB, or
preferably, an RPi runs companion software, [RF24Gateway](http://nRF24.github.io/RF24Gateway/), which creates a network interface linked to the RF24 radio network. This interface can be
further linked to the local network or internet. This allows the RPi or Arduino-based gateway to perform automatic discovery and routing of TCP/IP data,
with no required pre-configuration or interaction from the user beyond assigning appropriate addresses to the nodes initially.

## What does it do?
RF24Ethernet creates a network of internet enabled RF24/Arduino sensors. It provides an API and interface very similar to the Arduino Ethernet library,
to allow sensor nodes to connect out to local or internet based devices to retrieve or send information, or be connected to from the internet or your
local network directly.

Sensor nodes can act as individual web servers, simple command-and-control servers, or can connect out as required via TCP/IP.


## How does it work?
RF24Ethernet utilizes the UIP TCP/IP stack, allowing Arduino devices to use a Raspberry Pi running [RF24Gateway](http://nRF24.github.io/RF24Gateway/) or Arduino
as a gateway to your network or the internet, or simply as a repository for sensor information. The RF24, RF24Network and optionally RF24Mesh libraries
handle the underlying routing, addressing etc. so users do not need to be familiar with the radio modules or libraries.

RF24Network addresses need to be assigned as MAC addresses, and IP addresses can be configured as desired by the user. The master node (00) uses
either the Address Resolution Protocol (ARP) or RF24Mesh to find the appropriate nodes when IP traffic is sent though, and routes traffic to the correct
RF24Network address.

This provides a fairly seamless interaction, since users only need to configure standard IP forwarding and firewall rules as desired.

The RF24 libraries are based on the [OSI model](http://en.wikipedia.org/wiki/OSI_model):

| System                 | OSI Layer              | Description |
|------------------------|------------------------|---------------------------------------------------------------------------|
| <b>NRF24L01 Radio Modules</b> | OSI Layer 1 (Physical)   | Transmission and reception of the raw bit stream over the physical medium. |
| <b>RF24 Core Radio Driver</b> | OSI Layer 2 (Data Link)  | Transfer of data frames over the physical link. Establish/Terminate logical links between nodes. Manage acknowledgements, error checking |
| <b>RF24Network</b>            | OSI Layer 3 (Network)    | Routing, subnets, subnet traffic control, logical-to-physical address mapping, frame fragmentation/reassembly.|
| <b>RF24Ethernet (uIP)</b>     | OSI Layer 4 (Transport)  | Ensures error-free messages. Manages sequencing, losses, retries, and duplicates. |
| <b>RF24Ethernet (core)</b>    | OSI Layer 5 (Session)    | Establish, maintain, and terminate connections |
| <b>RF24Mesh</b> -Optional-    | OSI Layer 7 (Application)| Provides DHCP/DNS type protocols and allows dynamic addressing/topology for RF24Network.

### TAP vs TUN
RF24Ethernet and RF24Gateway are able to utilize both TAP and TUN type interfaces.

#### TAP
A TAP interface can be looked at much the same as an Ethernet interface, Ethernet headers are used to identify devices via MAC address, and the Address
Resolution Protocol (ARP) is used to perform that identification.

#### TUN
A TUN interface does not utilize Ethernet headers, MAC addresses or ARP, in this case relying on RF24Mesh/IP routing instead.

### Limitations
UDP frames can be up to 512 bytes in length, so UDP/DNS is limited by available memory, and the defined MAX_PAYLOAD_SIZE / UIP_BUFFER_SIZE. If a
DNS request exceeds the maximum allowed size, it will be dropped. DNS responses for addresses such as 'www.google.com' tend to be much smaller than requests
for 'google.com', and will generally work with the default configuration.

### MAC address formatting
RF24Ethernet uses a simple method of formatting the MAC addresses, using the first four bytes to store the characters 'RF24', and the last two bytes to store the
RF24Network address, which makes identification of nodes via MAC address very simple using standard monitoring tools.

| RF24Network Address| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 |
|--------------------|--------|--------|--------|--------|--------|--------|
|      n/a           |   R    |   F    |   2    |   4    |Octal LB|Octal HB|
|      01            |  0x52  |  0x46  |  0x32  |   0x34 |  0x01  |  0x00  |
|      011           |  0x52  |  0x46  |  0x32  |   0x34 |  0x09  |  0x00  |
|      0443          |  0x52  |  0x46  |  0x32  |   0x34 |  0x23  |  0x01  |
