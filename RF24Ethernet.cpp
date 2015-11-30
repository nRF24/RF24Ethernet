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

#include "RF24Ethernet.h"
				   
IPAddress RF24EthernetClass::_dnsServerAddress;
//DhcpClass* RF24EthernetClass::_dhcp(NULL);

/*************************************************************/
#if defined (RF24_TAP) 
RF24EthernetClass::RF24EthernetClass(RF24& _radio, RF24Network& _network): radio(_radio),network(_network)
	//fn_uip_cb(NULL)
{
}
#else // Using RF24Mesh
RF24EthernetClass::RF24EthernetClass(RF24& _radio, RF24Network& _network, RF24Mesh& _mesh): radio(_radio),network(_network), mesh(_mesh)
	//fn_uip_cb(NULL)
{
}
#endif
/*************************************************************/

void RF24EthernetClass::update() {
  Ethernet.tick();
}

/*************************************************************/
#if !defined ARDUINO_ARCH_ESP8266
void yield() {
  Ethernet.update();
}
#endif
/*************************************************************/

void RF24EthernetClass::use_device()
{
// Kept for backwards compatibility only
}

/*******************************************************/

/*******************************************************/
void RF24EthernetClass::setMac(uint16_t address){
	
	if(!network.multicastRelay){ // Radio has not been started yet
	  radio.begin();
	}	
	
	const uint8_t mac[6] = {0x52,0x46,0x32,0x34,address,address>>8};
	//printf("MAC: %o %d\n",address,mac[0]);
	
	#if defined (RF24_TAP)
	  uip_seteth_addr(mac);
   	  network.multicastRelay = 1;
	#endif
	RF24_Channel = RF24_Channel ? RF24_Channel : 97;
	network.begin(RF24_Channel, address);
}

/*******************************************************/

void RF24EthernetClass::setChannel(uint8_t channel){
	
	RF24_Channel = channel;
	if(network.multicastRelay){ // Radio has not been started yet
	  radio.setChannel(RF24_Channel);
	}
}

/*******************************************************/

void RF24EthernetClass::begin(IPAddress ip)
{
IPAddress dns = ip;
dns[3] = 1;
begin(ip, dns);
}

void RF24EthernetClass::begin(IPAddress ip, IPAddress dns)
{
IPAddress gateway = ip;
gateway[3] = 1;
begin(ip, dns, gateway);
}

void RF24EthernetClass::begin(IPAddress ip, IPAddress dns, IPAddress gateway)
{
IPAddress subnet(255, 255, 255, 0);
begin(ip, dns, gateway, subnet);
}

void RF24EthernetClass::begin(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet)
{
//init(mac);
configure(ip,dns,gateway,subnet);
}

/*******************************************************/

void RF24EthernetClass::configure(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet) {

  #if !defined (RF24_TAP) // Using RF24Mesh
    mesh.setNodeID(ip[3]);
  #endif
  
uip_buf = (uint8_t*)&network.frag_ptr->message_buffer[0];

uip_ipaddr_t ipaddr;
uip_ip_addr(ipaddr, ip);
uip_sethostaddr(ipaddr);
uip_ip_addr(ipaddr, gateway);
uip_setdraddr(ipaddr);
uip_ip_addr(ipaddr, subnet);
uip_setnetmask(ipaddr);
_dnsServerAddress = dns;

	timer_set(&this->periodic_timer, CLOCK_SECOND / UIP_TIMER_DIVISOR);
	//timer_set(&this->periodic_timer, CLOCK_SECOND / 4);
	
	#if defined (RF24_TAP)
//	timer_set(&this->arp_timer, CLOCK_SECOND * 10);
	timer_set(&this->arp_timer, CLOCK_SECOND * 2);
	#endif
	
	uip_init();
	#if defined (RF24_TAP)
	uip_arp_init();	
	#endif

}

/*******************************************************/

void RF24EthernetClass::set_gateway(IPAddress gwIP)
{
  uip_ipaddr_t ipaddr;
  uip_ip_addr(ipaddr, gwIP);
  uip_setdraddr(ipaddr);  
}

/*******************************************************/

void RF24EthernetClass::listen(uint16_t port)
{
  uip_listen(HTONS(port));
}

/*******************************************************/
 
IPAddress RF24EthernetClass::localIP() {
IPAddress ret;
uip_ipaddr_t a;
uip_gethostaddr(a);
return ip_addr_uip(a);
}

/*******************************************************/

IPAddress RF24EthernetClass::subnetMask() {
IPAddress ret;
uip_ipaddr_t a;
uip_getnetmask(a);
return ip_addr_uip(a);
}

/*******************************************************/

IPAddress RF24EthernetClass::gatewayIP() {
IPAddress ret;
uip_ipaddr_t a;
uip_getdraddr(a);
return ip_addr_uip(a);
}

/*******************************************************/

IPAddress RF24EthernetClass::dnsServerIP() {
return _dnsServerAddress;
}

/*******************************************************/

void RF24EthernetClass::tick() {
    #if defined (ARDUINO_ARCH_ESP8266)
      yield();
    #endif
	if(RF24Ethernet.network.update() == EXTERNAL_DATA_TYPE){
		uip_len = RF24Ethernet.network.frag_ptr->message_size;
	}

    #if !defined (RF24_TAP)
	if(uip_len > 0) {
	  uip_input();
	  if(uip_len > 0) {
	    network_send();	
	  }
	} else if(timer_expired(&Ethernet.periodic_timer)) {
      timer_reset(&Ethernet.periodic_timer);
      for(int i = 0; i < UIP_CONNS; i++) {
	    uip_periodic(i);
	    /* If the above function invocation resulted in data that
	    should be sent out on the network, the global variable
	    uip_len is set to a value > 0. */
	    if(uip_len > 0) {
	      network_send();
	    }
      }
	}
	#else
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
    #endif
#if UIP_UDP
      for(int i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  //uip_arp_out();
	  //network_send();
	  RF24UDP::_send((uip_udp_userdata_t *)(uip_udp_conns[i].appstate));
	}
      }
#endif /* UIP_UDP */
#if defined (RF24_TAP)      
      /* Call the ARP timer function every 10 seconds. */

	if(timer_expired(&Ethernet.arp_timer)) {
	  timer_reset(&Ethernet.arp_timer);
	  uip_arp_timer();
    }

  }

#endif //RF24_TAP
}


boolean RF24EthernetClass::network_send()
{
		RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
		//while(millis() - RF24Ethernet.lastRadio < 1){}

		  bool ok = RF24Ethernet.network.write(headerOut,uip_buf,uip_len);
		//#endif
		
		#if defined ETH_DEBUG_L1 || defined ETH_DEBUG_L2
		if(!ok){
		  Serial.println(); Serial.print(millis()); Serial.println(F(" *** RF24Ethernet Network Write Fail ***")); 
		}
		#endif
		#if defined ETH_DEBUG_L2
		if(ok){
		  Serial.println(); Serial.print(millis()); Serial.println(F(" RF24Ethernet Network Write OK")); 
		}
		#endif
		//RF24Ethernet.packetstate &= ~UIPETHERNET_SENDPACKET;
}

/*******************************************************/
/*
void uipudp_appcall(){

}*/

/*******************************************************/

