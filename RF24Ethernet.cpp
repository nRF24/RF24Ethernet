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
//#include "slipdev.h"
#include "timer.h"
}  

#include <RF24.h>
#include <RF24Network.h>
RF24 _radio(48,49);                // nRF24L01(+) radio attached using Getting Started board 
RF24Network _network(_radio);
// Because uIP isn't encapsulated within a class we have to use global
// variables, so we can only have one TCP/IP stack per program.  But at least
// we can set which serial port to use, for those boards with more than one.
//SerialDevice *slip_device;

RF24EthernetStack::RF24EthernetStack():fn_uip_cb(NULL){}

//RF24EthernetStack::RF24EthernetStack(RF24Network& _network): network(_network){}
	//fn_uip_cb(NULL)
//{
//}

void RF24EthernetStack::use_device()
{

  _radio.begin();
  _network.begin(97,01);
  _radio.setPALevel(RF24_PA_MIN);
  _radio.printDetails();
	//::slip_device = &_network;
	//network = _network;
}

void RF24EthernetStack::begin(IP_ADDR myIP, IP_ADDR subnet)
{
	uip_ipaddr_t ipaddr;

	//timer_set(&this->periodic_timer, CLOCK_SECOND / 4);
	timer_set(&this->periodic_timer, CLOCK_SECOND / 8);
	
	#if defined (TAP)
		timer_set(&this->arp_timer, CLOCK_SECOND * 10);
		
		//uip_seteth_addr(mac);
		uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
		uip_seteth_addr(mac);
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

void RF24EthernetStack::set_gateway(IP_ADDR myIP)
{
  uip_ipaddr_t ipaddr;
  uip_ipaddr(ipaddr, myIP.a, myIP.b, myIP.c, myIP.d);
  uip_setdraddr(ipaddr);
}

void RF24EthernetStack::listen(uint16_t port)
{
  uip_listen(HTONS(port));
}
    /*hwsend(&uip_buf[0], UIP_LLH_LEN);
    if(uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
      hwsend(&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN);
    } else {
      hwsend(&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
      hwsend(uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
	  
	 */ 
void RF24EthernetStack::tick()
{
	//uip_len = slipdev_poll();
	//_network.update();
	//RF24NetworkHeader header;
	RF24NetworkHeader headerOut(00,'EXTERNAL_DATA_TYPE');
	//uip_len = _network.peek(header);
	//_network.read(header,&uip_buf, uip_len);
	if(_network.update() == EXTERNAL_DATA_TYPE){
		RF24NetworkFrame *frame = _network.frag_ptr;
		memcpy(&uip_buf,frame->message_buffer,frame->message_size);
		//Serial.print("got len");
		/*Serial.println(frame->message_size);		
		for(int i=0; i<frame->message_size; i++){
			Serial.print(uip_buf[i],HEX);
		}
		Serial.println("");*/
		uip_len = frame->message_size;
	}
	if(uip_len > 0) {
	
	#if defined (TAP)
	  if(BUF->type == htons(UIP_ETHTYPE_IP)) {
		uip_arp_ipin();
		//Serial.println("TAP");
	#endif

		uip_input();
		// If the above function invocation resulted in data that
		// should be sent out on the network, the global variable
		// uip_len is set to a value > 0.
		if (uip_len > 0){
		    uip_arp_out();
			size_t totalSize = 0;
		    //uint8_t buffer[uip_len + UIP_LLH_LEN + UIP_TCPIP_HLEN];
			/*memcpy(&buffer,	&uip_buf,UIP_LLH_LEN);
			totalSize += UIP_LLH_LEN;
			if(uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
				memcpy(&buffer+totalSize,&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN);
				totalSize += uip_len - UIP_LLH_LEN;
			}else{
				memcpy(&buffer+totalSize,&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
				totalSize += UIP_TCPIP_HLEN;
				memcpy(&buffer+totalSize,uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
				totalSize += uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN;
			
			
			}*/
			//memcpy(&buffer,	&uip_buf,uip_len);
			_network.write(headerOut,&uip_buf,uip_len);
			//_network.write(headerOut,&buffer,totalSize);
		}
		
		

	#if defined (TAP)	
		 }else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
         uip_arp_arpin();	Serial.println("ARp");
         // If the above function invocation resulted in data that
         //   should be sent out on the network, the global variable
         //   uip_len is set to a value > 0. 
           if(uip_len > 0) {
           //network_device_send();
		   //uint8_t buffer[uip_len + UIP_LLH_LEN + UIP_TCPIP_HLEN];
		   //memcpy(&buffer,	&uip_buf,uip_len);
		   _network.write(headerOut,&uip_buf,uip_len);
           }
	    }
       
	#endif
	
	
	 }else if (timer_expired(&periodic_timer)) {
		timer_reset(&periodic_timer);
		for (int i = 0; i < UIP_CONNS; i++) {
			uip_periodic(i);
			// If the above function invocation resulted in data that
			// should be sent out on the network, the global variable
			// uip_len is set to a value > 0.
			if (uip_len > 0) {
			uip_arp_out();
			//uint8_t buffer[uip_len + UIP_LLH_LEN + UIP_TCPIP_HLEN];
			//memcpy(&buffer,	&uip_buf,uip_len);
			_network.write(headerOut,&uip_buf,uip_len);
			
			}
		}
	 


#if UIP_UDP
		for (int i = 0; i < UIP_UDP_CONNS; i++) {
			uip_udp_periodic(i);
			// If the above function invocation resulted in data that
			// should be sent out on the network, the global variable
			// uip_len is set to a value > 0. */
			if (uip_len > 0){
			//uint8_t buffer[uip_len + UIP_LLH_LEN + UIP_TCPIP_HLEN];
			//memcpy(&buffer,	&uip_buf,uip_len);
			_network.write(headerOut,&uip_buf,uip_len);
			}
		}
#endif /* UIP_UDP */
       /* Call the ARP timer function every 10 seconds. */
       if(timer_expired(&arp_timer)) {
         timer_reset(&arp_timer);
         uip_arp_timer();
       }
	}
	
}

void RF24EthernetStack::set_uip_callback(fn_uip_cb_t fn)
{
	this->fn_uip_cb = fn;
}

void RF24EthernetStack::uip_callback()
{
	struct serialip_state *s = &(uip_conn->appstate);
	//RF24Ethernet.cur_conn = s;
	if (this->fn_uip_cb) {
		// The sketch wants to handle all uIP events itself, using uIP functions.
		this->fn_uip_cb(s);//->p, &s->user);
	} else {
		// The sketch wants to use our simplified interface.
		// This is still in the planning stage :-)
		/*	struct serialip_state *s = &(uip_conn->appstate);

		SerialIP.cur_conn = s;

		if (uip_connected()) {
			s->obpos = 0;
			handle_ip_event(IP_INCOMING_CONNECTION, &s->user);
		}

		if (uip_rexmit() && s->obpos) {
			// Send the same buffer again
			SerialIP.queue();
			//uip_send(this->send_buffer, this->bufpos);
			return;
		}

		if (
			uip_closed() ||
			uip_aborted() ||
			uip_timedout()
		) {
			handle_ip_event(IP_CONNECTION_CLOSED, &s->user);
			return;
		}

		if (uip_acked()) {
			s->obpos = 0;
			handle_ip_event(IP_PACKET_ACKED, &s->user);
		}

		if (uip_newdata()) {
			handle_ip_event(IP_INCOMING_DATA, &s->user);
		}

		if (
			uip_newdata() ||
			uip_acked() ||
			uip_connected() ||
			uip_poll()
		) {
			// We've got space to send another packet if the user wants
			handle_ip_event(IP_SEND_PACKET, &s->user);
		}*/
	}
}

//RF24Network net = RF24EthernetStack::network;
RF24EthernetStack RF24Ethernet;

// uIP callback function
void serialip_appcall(void)
{
	RF24Ethernet.uip_callback();
}



/*
 * Code to interface the serial port with the SLIP handler.
 *
 * See slipdev.h for further explanation.
 */
/*
extern SerialDevice *slip_device;

// Put a character on the serial device.
void slipdev_char_put(u8_t c)
{
	::slip_device->write((char)c);
}

// Poll the serial device for a character.
u8_t slipdev_char_poll(u8_t *c)
{
	if (::slip_device->available()) {
		*c = ::slip_device->read();
		return 1;
	}
	return 0;
}*/
