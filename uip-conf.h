/**
 * \name Project-specific configuration options
 * @{
 *
 * uIP has a number of configuration options that can be overridden
 * for each project. These are kept in a project-specific uip-conf.h
 * file and all configuration names have the prefix UIP_CONF.
 */

/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 */

#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <inttypes.h>

/**
 * 8 bit datatype
 *
 * This typedef defines the 8-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint8_t u8_t;

/**
 * 16 bit datatype
 *
 * This typedef defines the 16-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint16_t u16_t;

/**
 * Statistics datatype
 *
 * This typedef defines the dataype used for keeping statistics in
 * uIP.
 *
 * \hideinitializer
 */
typedef unsigned short uip_stats_t;



/********** TMRh20: This option is not yet valid **********/
/* for TCP */
#define UIP_SOCKET_NUMPACKETS    1


/************* TMRh20: User Configuration *******************/

/**
 * Maximum number of TCP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_CONNECTIONS 1

/**
 * Maximum number of listening TCP ports.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_LISTENPORTS 1

/**
 * uIP buffer size.
 *
 * \hideinitializer
 */

#define UIP_CONF_BUFFER_SIZE     120

 /**
  * Adjust the rate at which the IP stack performs periodic processing.  
  * The periodic timer will be called at a rate of 1 second divided by this value  
  *  
  * Default: 500ms  
  * Increase this value to reduce response times and increase throughput during user interactions.  
  * @note: Increasing this value will increase network traffic and errors.  
  */
#define UIP_TIMER_DIVISOR 3


 /**
  * Optional: Uncomment to disable  
  *
  * Adjust the length of time after which a connection will be timed out.  
  * 
  * If data is not sent or received on an open connection for this duration in ms, kill the connection.
  */
#define UIP_CONNECTION_TIMEOUT 20000

/**
 *  SLIP/TUN - 14 for Ethernet/TAP & 0 for TUN
 */
#define UIP_CONF_LLH_LEN 14

/**
* uIP User Output buffer size 
* The output buffer size determines the max 
* length of strings that can be sent by the user
* Must be <=   UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
*/
//#define OUTPUT_BUFFER_SIZE 45
#define OUTPUT_BUFFER_SIZE UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN

/******************** END USER CONFIG ***********************************/

/**
 * The TCP maximum segment size.
 *
 * This is should not be to set to more than
 * UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN.
 */
//#define UIP_CONF_RECEIVE_WINDOW 60 //This is set automatically to the max allowable size
/**
 * CPU byte order.
 *
 * \hideinitializer
 */
#define UIP_CONF_BYTE_ORDER      LITTLE_ENDIAN

/**
 * Logging on or off
 *
 * \hideinitializer
 */
//#define UIP_CONF_LOGGING         1
#define UIP_CONF_LOGGING         0

/**
 * UDP support on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP             0
//#define UIP_CONF_BROADCAST       0
//#define UIP_CONF_UDP_CONNS       1
/**
 * UDP checksums on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CHECKSUMS   0

/**
 * uIP statistics on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_STATISTICS      0

// Define config for TAP or TUN based on Link-layer header length
#if UIP_CONF_LLH_LEN > 0
  #define RF24_TAP
#endif

#if defined UIP_TIMER_DIVISOR
  #define UIP_CONF_RTO (UIP_TIMER_DIVISOR*2)-1
#endif

typedef void* uip_tcp_appstate_t;

void serialip_appcall(void);
#define UIP_APPCALL serialip_appcall

typedef void* uip_udp_appstate_t;
void uipudp_appcall(void);
#define UIP_UDP_APPCALL uipudp_appcall


#endif /* __UIP_CONF_H__ */

/** @} */
