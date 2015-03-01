/*
 RF24UDP.cpp - Arduino implementation of a uIP wrapper class.
 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "RF24Ethernet.h"

#if UIP_CONF_UDP > 0

#ifdef RF24ETHERNET_DEBUG_UDP
#include "HardwareSerial.h"
#endif


#if UIP_UDP
#define UIP_ARPHDRSIZE 42
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

/*******************************************************/

// Constructor
RF24UDP::RF24UDP() :
    _uip_udp_conn(NULL)
{
  memset(&appdata,0,sizeof(appdata));
}

/*******************************************************/

// initialize, start listening on specified port. Returns 1 if successful, 0 if there are no sockets available to use
uint8_t RF24UDP::begin(uint16_t port) {
  if (!_uip_udp_conn) {
      _uip_udp_conn = uip_udp_new(NULL, 0);
  }
  if (_uip_udp_conn) {
      uip_udp_bind(_uip_udp_conn,htons(port));
      _uip_udp_conn->appstate = &appdata;
      return 1;
  }
  return 0;
}

/*******************************************************/

// Finish with the UDP socket
void RF24UDP::stop() {
  if (_uip_udp_conn) {
      uip_udp_remove(_uip_udp_conn);
      _uip_udp_conn->appstate = NULL;
      _uip_udp_conn=NULL;
	  appdata.packet_in = 0;
	  appdata.packet_next = 0;
	  appdata.packet_out = 0;
	  
      memset(&appdata,0,sizeof(appdata));
  }
}

/*******************************************************/

// Sending UDP packets

// Start building up a packet to send to the remote host specific in ip and port
// Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
int RF24UDP::beginPacket(IPAddress ip, uint16_t port){
  RF24EthernetClass::tick();
  if (ip && port) {
      uip_ipaddr_t ripaddr;
      uip_ip_addr(&ripaddr, ip);
      IF_RF24ETHERNET_DEBUG_UDP( Serial.print(F("RF24UDP udp beginPacket, ")); );

      if (_uip_udp_conn){
          _uip_udp_conn->rport = htons(port);
          uip_ipaddr_copy(_uip_udp_conn->ripaddr, &ripaddr);
      }else{
          _uip_udp_conn = uip_udp_new(&ripaddr,htons(port));
          if (_uip_udp_conn) {
              IF_RF24ETHERNET_DEBUG_UDP( Serial.print(F("RF24UDP New connection, ")); );
              _uip_udp_conn->appstate = &appdata;
          }else{
              IF_RF24ETHERNET_DEBUG_UDP( Serial.println(F("RF24UDP Failed to allocate new connection")); );
              return 0;
          }
      }
          IF_RF24ETHERNET_DEBUG_UDP( Serial.print(F("rip: ")); Serial.print(ip); Serial.print(F(", port: ")); Serial.println(port); );
  }

  if (_uip_udp_conn) {
      if (appdata.packet_out == 0) {
          appdata.packet_out = 1;
          appdata.out_pos = 0;//UIP_UDP_PHYH_LEN;
          if (appdata.packet_out != 0){
            return 1;          
		  }else{
		    IF_RF24ETHERNET_DEBUG_UDP( Serial.println(F("RF24UDP Failed to allocate memory for packet")); );
          }
	  }else{
	    IF_RF24ETHERNET_DEBUG_UDP( Serial.println(F("RF24UDP Previous packet on that connection not sent yet")); );
	  }
  }
  return 0;
}

/*******************************************************/

// Start building up a packet to send to the remote host specific in host and port
// Returns 1 if successful, 0 if there was a problem resolving the hostname or port
int RF24UDP::beginPacket(const char *host, uint16_t port) {
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(RF24Ethernet.dnsServerIP());
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return beginPacket(remote_addr, port);
  } else {
    return ret;
  }
}

/*******************************************************/

// Finish off this packet and send it
// Returns 1 if the packet was sent successfully, 0 if there was an error
int RF24UDP::endPacket() {
  if (_uip_udp_conn && appdata.packet_out != 0) {
      appdata.send = true;
	  IF_RF24ETHERNET_DEBUG_UDP( Serial.println(F("RF24UDP endpacket")); );
      uip_udp_periodic_conn(_uip_udp_conn);
      if (uip_len > 0) {
          _send(&appdata);
          return 1;
        }
  }
  return 0;
}

/*******************************************************/

// Write a single byte into the packet
size_t RF24UDP::write(uint8_t c) {
  return write(&c,1);
}

/*******************************************************/

// Write size bytes from buffer into the packet
size_t RF24UDP::write(const uint8_t *buffer, size_t size) {
  if (appdata.packet_out != 0) {
	
	  IF_RF24ETHERNET_DEBUG_UDP( Serial.println("RF24UDP Write: "); Serial.println(size); for(int i=0; i<size; i++){ Serial.print((char)buffer[i]); Serial.print(" "); } Serial.println(""); );
	  size_t ret = size;
	  memcpy(RF24Client::all_data[0].myData + appdata.out_pos ,buffer,size);
      appdata.out_pos += ret;
      return ret;
  }
  return 0;
}

/*******************************************************/

// Start processing the next available incoming packet
// Returns the size of the packet in bytes, or 0 if no packets are available
int RF24UDP::parsePacket() {

  RF24EthernetClass::tick();
  int size = appdata.packet_in_size;
  
  IF_RF24ETHERNET_DEBUG_UDP( if (appdata.packet_in != 0) { Serial.print(F("RF24UDP udp parsePacket freeing previous packet: ")); Serial.println(appdata.packet_in); } );

  //appdata.packet_in_size = 0;
  
  //appdata.packet_in = appdata.packet_next;
  //appdata.packet_next = 0;

  IF_RF24ETHERNET_DEBUG_UDP( if (appdata.packet_in != 0) { Serial.print(F("RF24UDP udp parsePacket received packet: ")); Serial.print(appdata.packet_in); } Serial.print(F(", size: ")); Serial.println(size); );
  
  return size;
}

/*******************************************************/

// Number of bytes remaining in the current packet
int RF24UDP::available() {
  RF24EthernetClass::tick();
  return appdata.packet_in_size;
}

/*******************************************************/

// Read a single byte from the current packet
int RF24UDP::read() {
  unsigned char c;
  if (read(&c,1) > 0) {
      return c;
  }
  return -1;
}

/*******************************************************/

// Read up to len bytes from the current packet and place them into buffer
// Returns the number of bytes read, or 0 if none are available
int RF24UDP::read(unsigned char* buffer, size_t len) {
  
  RF24EthernetClass::tick();
  
  if (appdata.packet_in != 0) {
	  memcpy(buffer,RF24Client::all_data[0].myDataIn + appdata.in_pos,len);
	  appdata.in_pos += len;
	  appdata.packet_in_size -= len;
	  
      if (appdata.packet_in_size < 1) {
          appdata.packet_in = 0;
      }
      return len;
  }
  return 0;
}

/*******************************************************/

// Return the next byte from the current packet without moving on to the next byte
int RF24UDP::peek() {

  RF24EthernetClass::tick();

  if (appdata.packet_in != 0) {
      return RF24Client::all_data[0].myDataIn[appdata.in_pos];
  }
  return -1;
}

/*******************************************************/

// Finish reading the current packet
void RF24UDP::flush() {
  appdata.packet_in = 0;
  appdata.packet_in_size = 0;
  RF24EthernetClass::tick();
}

/*******************************************************/

// Return the IP address of the host who sent the current incoming packet
IPAddress RF24UDP::remoteIP() {
  return _uip_udp_conn ? ip_addr_uip(_uip_udp_conn->ripaddr) : IPAddress();
}

/*******************************************************/

// Return the port of the host who sent the current incoming packet
uint16_t RF24UDP::remotePort() {
  return _uip_udp_conn ? ntohs(_uip_udp_conn->rport) : 0;
}

/*******************************************************/

// uIP callback function

void uipudp_appcall(void) {
  if (uip_udp_userdata_t *data = (uip_udp_userdata_t *)(uip_udp_conn->appstate)) {
      if (uip_newdata()) {
          if (data->packet_next == 0) {
              uip_udp_conn->rport = UDPBUF->srcport;
              uip_ipaddr_copy(uip_udp_conn->ripaddr,UDPBUF->srcipaddr);
			  
              //discard Linklevel and IP and udp-header and any trailing bytes:
			  memcpy(RF24Client::all_data[0].myDataIn,uip_appdata ,uip_len);
			  data->packet_in_size += uip_len;
			  data->packet_in = 1;
              
              IF_RF24ETHERNET_DEBUG_UDP( Serial.print(F("RF24UDP udp, uip_newdata received packet: ")); Serial.print(data->packet_next); Serial.print(F(", size: ")); Serial.println(data->packet_in_size); for(int i=0; i<data->packet_in_size; i++){  Serial.print(RF24Client::all_data[0].myDataIn[i],HEX); Serial.print(F(" : "));  } Serial.println();  );
          }
      }
      if (uip_poll() && data->send) {
          //set uip_slen (uip private) by calling uip_udp_send
          IF_RF24ETHERNET_DEBUG_UDP(Serial.print(F("udp, uip_poll preparing packet to send: ")); Serial.print(data->packet_out); Serial.print(F(", size: ")); Serial.println(data->out_pos);  );
		  
		  memcpy(uip_appdata,RF24Client::all_data[0].myData,data->out_pos);
          uip_udp_send(data->out_pos );
        }
    }
}

/*******************************************************/

void RF24UDP::_send(uip_udp_userdata_t *data) {
  #if defined (RF24_TAP)
  uip_arp_out(); //add arp
  #endif
  if (uip_len == UIP_ARPHDRSIZE) {
      //RF24EthernetClass::uip_packet = 0;
      //RF24EthernetClass::packetstate &= ~UIPETHERNET_SENDPACKET;

      IF_RF24ETHERNET_DEBUG_UDP(  Serial.println(F("udp, uip_poll results in ARP-packet")); );
	  RF24EthernetClass::network_send();
  }else{
  //arp found ethaddr for ip (otherwise packet is replaced by arp-request)
      data->send = false;
      data->packet_out = 0;
      //RF24EthernetClass::packetstate |= UIPETHERNET_SENDPACKET;
      IF_RF24ETHERNET_DEBUG_UDP( Serial.println(data->out_pos); Serial.print(F("udp, uip_packet to send: ")); for(int i=0; i<data->out_pos; i++){  Serial.print((char)RF24Client::all_data[0].myData[i]);  }  Serial.println("");  );
	  
      RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);  
      RF24Ethernet.network.write(headerOut,uip_buf,data->out_pos+UIP_UDP_PHYH_LEN);
  }
}

/*******************************************************/

#endif
#endif