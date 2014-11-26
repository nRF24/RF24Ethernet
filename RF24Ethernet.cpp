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

//RF24EthernetClass::RF24EthernetClass(){}//:fn_uip_cb(NULL){}

RF24EthernetClass::RF24EthernetClass(RF24& _radio, RF24Network& _network): radio(_radio),network(_network)
	//fn_uip_cb(NULL)
{

}

void RF24EthernetClass::use_device()
{
  radio.begin();
  RF24_Channel = RF24_Channel ? RF24_Channel : 97;
  //network.begin(Ethernet.RF24_Channel,04444);
}

/*******************************************************/

void RF24EthernetClass::setMac(uint16_t address){
	
	uint8_t mac[6] = {0x00,0x00,0x52,0x46,0x32,0x34};
	mac[0] = address;
	mac[1] = address >> 8;
	printf("MAC: %o %d\n",address,mac[0]);
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

	//timer_set(&this->periodic_timer, CLOCK_SECOND / 4);
	timer_set(&this->periodic_timer, CLOCK_SECOND / 8);
	
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
}

/*******************************************************/

void RF24EthernetClass::listen(uint16_t port)
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
	 

/*******************************************************/

void RF24EthernetClass::tick()
{


	if(RF24Ethernet.network.update() == EXTERNAL_DATA_TYPE){

		RF24NetworkFrame *frame = RF24Ethernet.network.frag_ptr;
		memcpy(&uip_buf,frame->message_buffer,frame->message_size);
    	#if defined (ETH_DEBUG_L1)
		Serial.print("got len");
		Serial.println(frame->message_size);		
		for(int i=0; i<frame->message_size; i++){
			Serial.print(uip_buf[i],HEX);
		}
		Serial.println("");
		#endif
		uip_len = frame->message_size;
	}
	if(uip_len > 0) {
	   //Serial.println("data in");
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
			//delay(5);
			bool ook = 0;
			//while(!ook){
			    RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
				ook = RF24Ethernet.network.write(headerOut,&uip_buf,uip_len);
			//}
			#if defined (ETH_DEBUG_L1)
			printf("data out %d\n",ook);
			#endif
			//network.write(headerOut,&buffer,totalSize);
		}
		
		

	#if defined (TAP)	
		 }else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
         uip_arp_arpin();	
		 #if defined (ETH_DEBUG_L1)
		 Serial.println("ARp");
		 #endif
         // If the above function invocation resulted in data that
         //   should be sent out on the network, the global variable
         //   uip_len is set to a value > 0. 
           if(uip_len > 0) {
           //network_device_send();
		   //uint8_t buffer[uip_len + UIP_LLH_LEN + UIP_TCPIP_HLEN];
		   //memcpy(&buffer,	&uip_buf,uip_len);
		   //delay(5);
			bool ook = 0;
			//while(!ook){
				RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
				ook = RF24Ethernet.network.write(headerOut,&uip_buf,uip_len);
			//}
		   #if defined (ETH_DEBUG_L1)
		   printf("data out %d\n",ook);
		   #endif
           }
	    }
       
	#endif
	
	
	 }else if (timer_expired(&RF24Ethernet.periodic_timer)) {
		timer_reset(&RF24Ethernet.periodic_timer);
		for (int i = 0; i < UIP_CONNS; i++) {
			uip_periodic(i);
			// If the above function invocation resulted in data that
			// should be sent out on the network, the global variable
			// uip_len is set to a value > 0.
			if (uip_len > 0) {
			uip_arp_out();
			//uint8_t buffer[uip_len + UIP_LLH_LEN + UIP_TCPIP_HLEN];
			//memcpy(&buffer,	&uip_buf,uip_len);
			//delay(5);
			bool ook = 0;
			//while(!ook){
				RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
				ook = RF24Ethernet.network.write(headerOut,&uip_buf,uip_len);
			//}
			#if defined (ETH_DEBUG_L1)
			printf("data out %d\n",ook);
			#endif
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
			//delay(5);
			bool ook = 0;
			while(!ook){
				RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
				ook = RF24Ethernet.network.write(headerOut,&uip_buf,uip_len);
			}
			//printf("data out %d",ook);
			}
		}
#endif /* UIP_UDP */
       /* Call the ARP timer function every 10 seconds. */
       if(timer_expired(&RF24Ethernet.arp_timer)) {
         timer_reset(&RF24Ethernet.arp_timer);
         uip_arp_timer();
       }
	}
	
}
/*
void RF24EthernetClass::set_uip_callback(fn_uip_cb_t fn)
{
	this->fn_uip_cb = fn;
}
*/

/*******************************************************/

void uipudp_appcall(){

}

/*******************************************************/



	
	//RF24Ethernet.cur_conn = s;
	//if (this->fn_uip_cb) {
		// The sketch wants to handle all uIP events itself, using uIP functions.
	//	this->fn_uip_cb(s);//->p, &s->user);
	//} else {
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
	//}


//RF24Network net = RF24EthernetClass::network;
//RF24EthernetClass RF24Ethernet;
/*		typedef struct connDataStruct{
			char input_buffer[16];
			char name[20];
		} connection_data;
		
		*/
		
// uIP callback function
//void serialip_appcall(void)
//{
//	RF24Ethernet.uip_callback();
	//uip_callback();
/*	 if (uip_connected()) {

    // We want to store some data in our connections, so allocate some space
    // for it.  The connection_data struct is defined in a separate .h file,
    // due to the way the Arduino IDE works.  (typedefs come after function
    // definitions.)
    connection_data *d = (connection_data *)malloc(sizeof(connection_data));

    // Save it as RF24Ethernet user data so we can get to it later.
    s->user = d;

    // The protosocket's read functions need a per-connection buffer to store
    // data they read.  We've got some space for this in our connection_data
    // structure, so we'll tell uIP to use that.
    PSOCK_INIT(&s->p, d->input_buffer, sizeof(d->input_buffer));

  }

  // Call/resume our protosocket handler.
  handle_connection(s, (connection_data *)s->user);

  // If the connection has been closed, release the data we allocated earlier.
  if (uip_closed()) {
    if (s->user) free(s->user);
    s->user = NULL;
  }*/
//}



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
