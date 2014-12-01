/*
  RF24Ethernet - Initially based on SerialIP
  
  SerialIP.h - Arduino implementation of a uIP wrapper class.
  Copyright (c) 2010 Adam Nielsen <malvineous@shikadi.net>
  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef RF24Ethernet_h
#define RF24Ethernet_h


#include <Arduino.h>
#include "ethernet_comp.h"
#include "IPAddress.h"
#include "RF24Client.h"
#include "RF24Server.h"
#include "RF24Ethernet_config.h"

extern "C" {
#include "utility/timer.h"
#include "utility/uip.h"
}


#define UIPETHERNET_FREEPACKET 1
#define UIPETHERNET_SENDPACKET 2

//#define TUN  // Use only the tcp protocol, no ethernet headers or arps
#define TAP  // Include ethernet headers

#if defined (TAP)
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#endif
//#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])	

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)
#define uip_ip_addr(addr, ip) do { \
                     ((u16_t *)(addr))[0] = HTONS(((ip[0]) << 8) | (ip[1])); \
                     ((u16_t *)(addr))[1] = HTONS(((ip[2]) << 8) | (ip[3])); \
                  } while(0)

#define ip_addr_uip(a) IPAddress(a[0] & 0xFF, a[0] >> 8 , a[1] & 0xFF, a[1] >> 8) //TODO this is not IPV6 capable

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)

						  

typedef struct {
	int a, b, c, d;
} IP_ADDR;

/*
#define IP_INCOMING_CONNECTION  0
#define IP_CONNECTION_CLOSED    1
#define IP_PACKET_ACKED         2
#define IP_INCOMING_DATA        3
#define IP_SEND_PACKET          4
*/
//typedef struct psock ip_connection_t;

//typedef void (*fn_uip_cb_t)(uip_tcp_appstate_t *conn);

//typedef void (*fn_my_cb_t)(unsigned long a);
//extern fn_my_cb_t x;

// Since this is a HardwareSerial class it means you can't use
// SoftwareSerial devices with it, but this could be fixed by making both
// HardwareSerial and SoftwareSerial inherit from a common Serial ancestor
// which we call SerialDevice here.
//typedef HardwareSerial SerialDevice;

class RF24;
class RF24Network;


class RF24EthernetClass {//: public Print {
	public:
		RF24& radio;
		RF24Network& network;
		RF24EthernetClass(RF24& _radio,RF24Network& _network);
		RF24EthernetClass();
		
		void use_device();
		void begin(IP_ADDR myIP, IP_ADDR subnet);
		void set_gateway(IP_ADDR myIP);
		void listen(uint16_t port);

		// Set a user function to handle raw uIP events as they happen.  The
		// callback function can only use uIP functions, but it can also use uIP's
		// protosockets.
		//void set_uip_callback(fn_uip_cb_t fn);

		/* Sets the MAC address of the RF24 module, which is an RF24Network address
		* Specify an Octal address to assign to this node, which will be used as the ethernet mac address
		* If setting up only a few nodes, use 01 to 05
		* Please reference the RF24Network documentation for information on setting up a static network
		* RF24Mesh will be integrated to provide this automatically
		*/
		void setMac(uint16_t address);
		
		/* Sets the Radio channel/frequency to use
		*/
		void setChannel(uint8_t channel);
		
		// Returns the number of bytes left in the send buffer.  When this reaches
		// zero all write/print data will be discarded.  Call queue() and return
		// from handle_ip_event() to send the data.  handle_ip_event() will be
		// called with IP_SEND_NEXT_PACKET when the packet has been received by the
		// remote host and you can send more data.
		//int sendbuffer_space();

		// Returns true when the sendbuffer is empty and ready for another packet.
		// Not necessary to use if you only send packets in response to
		// handle_ip_event(IP_SEND_NEXT_PACKET) and you use queue() when you're
		// done.
		//int sendbuffer_ready();

		// Don't mix this with write() in the same IP_SEND_NEXT_PACKET call.
		// This is *way* more efficient than using write() and queue() anyway.
		//void queueBuffer(uint8_t buf, int len);
		//void queueString(const char *buf);

		// Queue up the current sendbuffer.  It will be sent and flushed at the
		// next available opportunity.
		//void queue();

		// Print methods
		//virtual void write(uint8_t ch);
		/*virtual void write(const char *str);
		virtual void write(const uint8_t *buffer, size_t size);*/
	uint8_t myData[UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN];
	
	size_t dataCnt;
	int available();
	uint8_t packetstate;
	uint8_t uip_hdrlen;
	
	
	
	private:

		uint8_t in_packet;
		// tick() must be called at regular intervals to process the incoming serial
		// data and issue IP events to the sketch.  It does not return until all IP
		// events have been processed.
		static void tick();
		static boolean network_send();		
		
		uint8_t RF24_Channel;
		int handle_connection(uip_tcp_appstate_t *s);
		struct timer periodic_timer;
		
		struct timer arp_timer;
		void uip_callback();

	//friend void serialip_appcall(void);
	friend void uipudp_appcall(void);
    
	friend class RF24Server;
    friend class RF24Client;

};

//void handle_ip_event(uint8_t type, ip_connection_t *conn, void **user);

extern RF24EthernetClass RF24Ethernet;


#endif
