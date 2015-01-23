/*
 UIPServer.cpp - Arduino implementation of a uIP wrapper class.
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
#include "RF24Server.h"

extern "C" {
  //#include "uip-conf.h"
}

/*************************************************************/

RF24Server::RF24Server(uint16_t port) : _port(htons(port))
{
}

/*************************************************************/

RF24Client RF24Server::available()
{
  Ethernet.tick();
  for ( uip_userdata_t* data = &RF24Client::all_data[0]; data < &RF24Client::all_data[UIP_CONNS]; data++ )
    {
        if (data->packets_in != 0 && (((data->state & UIP_CLIENT_CONNECTED) && uip_conns[data->state & UIP_CLIENT_SOCKETS].lport ==_port)
              || ((data->state & UIP_CLIENT_REMOTECLOSED) && ((uip_userdata_closed_t *)data)->lport == _port))){
			  return RF24Client(data);
		}
    }	
  return RF24Client();
}

/*************************************************************/

void RF24Server::begin()
{  
  uip_listen(_port);
  RF24Ethernet.tick();  
}

/*************************************************************/

size_t RF24Server::write(uint8_t c)
{
  return write(&c,1);
}

/*************************************************************/

size_t RF24Server::write(const uint8_t *buf, size_t size)
{
  size_t ret = 0;
  for ( uip_userdata_t* data = &RF24Client::all_data[0]; data < &RF24Client::all_data[UIP_CONNS]; data++ )
    {
      if ((data->state & UIP_CLIENT_CONNECTED) && uip_conns[data->state & UIP_CLIENT_SOCKETS].lport ==_port)
        ret += RF24Client::_write(data,buf,size);
    }
  return ret;
}

/*************************************************************/
