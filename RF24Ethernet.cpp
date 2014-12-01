/*
  RF24Ethernet - Initially based on SerialIP
  
  SerialIP.cpp - Arduino implementation of a uIP wrapper class.
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

#include <Arduino.h>
#include "RF24Ethernet.h"


extern "C" {
#include "uip-conf.h"
#include "uip.h"
#include "uip_arp.h"
#include "timer.h"
}  

#include <RF24.h>
#include <RF24Network.h>

/*************************************************************/

RF24EthernetClass::RF24EthernetClass(RF24& _radio, RF24Network& _network): radio(_radio),network(_network)
	//fn_uip_cb(NULL)
{
}

/*************************************************************/

void RF24EthernetClass::use_device()
{
  radio.begin();
  RF24_Channel = RF24_Channel ? RF24_Channel : 97;
}

/*******************************************************/

void RF24EthernetClass::setMac(uint16_t address){
	
	uint8_t mac[6] = {0x00,0x00,0x52,0x46,0x32,0x34};
	mac[0] = address;
	mac[1] = address >> 8;
	//printf("MAC: %o %d\n",address,mac[0]);
	uip_seteth_addr(mac);
	
	RF24_Channel = RF24_Channel ? RF24_Channel : 97;
	network.begin(RF24_Channel, address);
}

/*******************************************************/

void RF24EthernetClass::setChannel(uint8_t channel){
	
	RF24_Channel = channel;
	radio.setChannel(RF24_Channel);
}

/*******************************************************/

void RF24EthernetClass::begin(IP_ADDR myIP, IP_ADDR subnet)
{
	uip_ipaddr_t ipaddr;

	timer_set(&this->periodic_timer, CLOCK_SECOND / 4);
	//timer_set(&this->periodic_timer, CLOCK_SECOND / 4);
	
	#if defined (TAP)
		timer_set(&this->arp_timer, CLOCK_SECOND * 10);
		
		//uip_seteth_addr(mac);
		//52 46 32 34
		//uint8_t mac[6] = {0x01,0x00,0x52,0x46,0x32,0x34};
		//uip_seteth_addr(mac);
		uip_init();
		uip_arp_init();
		
	#else
	  uip_init();
	#endif
	
	//slipdev_init();
	uip_ipaddr(ipaddr, myIP.a, myIP.b, myIP.c, myIP.d);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, subnet.a, subnet.b, subnet.c, subnet.d);
	uip_setnetmask(ipaddr);

}

/*******************************************************/

void RF24EthernetClass::set_gateway(IP_ADDR myIP)
{
  uip_ipaddr_t ipaddr;
  uip_ipaddr(ipaddr, myIP.a, myIP.b, myIP.c, myIP.d);
  uip_setdraddr(ipaddr);
/*  uip_getdraddr(ipaddr);
  Serial.println(uip_ipaddr1(&ipaddr));
  Serial.println(uip_ipaddr2(&ipaddr));
  Serial.println(uip_ipaddr3(&ipaddr));
  Serial.println(uip_ipaddr4(&ipaddr));*/
  
}

/*******************************************************/

void RF24EthernetClass::listen(uint16_t port)
{
  uip_listen(HTONS(port));
}
 

/*******************************************************/


void RF24EthernetClass::tick() {

	if(RF24Ethernet.network.update() == EXTERNAL_DATA_TYPE){
		RF24NetworkFrame *frame = RF24Ethernet.network.frag_ptr;	
		uip_len = frame->message_size;
		memcpy(&uip_buf,frame->message_buffer,frame->message_size);		
	}else{
		uip_len = 0;
	}


    if(uip_len > 0) {
      if(BUF->type == htons(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  network_send();
	}
      } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  network_send();
	}
      }

    } else if(timer_expired(&Ethernet.periodic_timer)) {
      timer_reset(&Ethernet.periodic_timer);
      for(int i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  network_send();
	}
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  network_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&Ethernet.arp_timer)) {
	timer_reset(&Ethernet.arp_timer);
	uip_arp_timer();
      }
    }
}


boolean RF24EthernetClass::network_send()
{
	RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
	
	bool ok = 0;
	if(RF24Ethernet.packetstate == UIPETHERNET_SENDPACKET){
		
		uint8_t buf[uip_len + UIP_TCPIP_HLEN + UIP_LLH_LEN];
		size_t pSize = 0;
		//RF24Ethernet.network.write(headerOut,&uip_buf[0], UIP_LLH_LEN);
		memcpy(&buf[0],&uip_buf[0],UIP_LLH_LEN);
		pSize += UIP_LLH_LEN;
		if(uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
			//RF24Ethernet.network.write(headerOut,&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN);
			memcpy(&buf[UIP_LLH_LEN],&uip_buf[UIP_LLH_LEN],uip_len-UIP_LLH_LEN);
			pSize += uip_len-UIP_LLH_LEN;
		} else {
			//RF24Ethernet.network.write(headerOut,&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
			//RF24Ethernet.network.write(headerOut,uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
			memcpy(&buf[UIP_LLH_LEN],&uip_buf[UIP_LLH_LEN],UIP_TCPIP_HLEN);
			pSize += UIP_TCPIP_HLEN;
			memcpy(&buf[UIP_LLH_LEN+UIP_TCPIP_HLEN],uip_appdata,uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
			pSize += uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN;
		}
		ok = RF24Ethernet.network.write(headerOut,&buf[0],pSize);
	}else{
 
		
		ok = RF24Ethernet.network.write(headerOut,&uip_buf,uip_len);
	}
	if(ok){
		RF24Ethernet.packetstate &= ~UIPETHERNET_SENDPACKET;
	}
}

/*******************************************************/

void uipudp_appcall(){

}

/*******************************************************/

