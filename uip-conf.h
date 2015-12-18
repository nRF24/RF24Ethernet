/** @file uip-conf.h*/
/**
 * \name User configuration options
 * @{
 *
 * uIP has a number of configuration options that can be overridden
 * for each project. These are kept in a project-specific uip-conf.h
 * file and all configuration names have the prefix UIP_CONF.
 * Some of these options are specific to RF24Ethernet.
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
 * Modified 2015: TMRh20
 */

#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <inttypes.h>

#include "RF24Network_config.h"
/************* TMRh20: User Configuration *******************/
/**
@defgroup UipConfiguration

User Configuration Options
/* @{ */

/** Maximum number of TCP connections. */
#define UIP_CONF_MAX_CONNECTIONS 1

/** Maximum number of listening TCP ports. */
#define UIP_CONF_MAX_LISTENPORTS 1

/**
 * uIP buffer size.
 * @note For simplicity, this is automatically set to the MAX_PAYLOAD_SIZE configured in the RF24Network_conf.h file, but can be configured independently
 * of RF24Network if desired.
 * 
 * Notes:
 * 1. Nodes can use different buffer sizes, direct TCP communication is limited to the smallest  
 *  ie: A RPi can be configured to use 1500byte TCP windows, with Arduino nodes using only 120byte TCP windows.
 * 2. Routing nodes handle traffic at the link-layer, so the MAX_PAYLOAD_SIZE is not important, unless they are
 * running RF24Ethernet.  
 * 3. Nodes running RF24Ethernet generally do not need to support RF24Network user payloads. Edit RF24Network_config.h  
 *  and uncomment #define DISABLE_USER_PAYLOADS. This will free memory not used with RF24Ethernet. 
 * 4. The user buffers are automatically configured to (Buffer Size - Link Layer Header Size - TCP Header Size) so
 * using RF24Mesh will decrease payloads by 14 bytes.
 * 5. Larger buffer sizes increase throughput for individual nodes, but can impact other network traffic.
 * 6. Max usable value is 512
 */

#define UIP_CONF_BUFFER_SIZE MAX_PAYLOAD_SIZE
/**
  * <b>Optional:</b> Uncomment to disable  
  *
  * Adjust the length of time after which an open connection will be timed out.  
  * 
  * If uIP is polling the established connection, but an ack or data is not received for this duration in ms, kill the connection.
  */
#define UIP_CONNECTION_TIMEOUT 30000

/**
 * SLIP/TUN - 14 for Ethernet/TAP & 0 for TUN/SLIP  
 *  
 * Ethernet headers add an additional 14 bytes to each payload.  
 *  
 * RF24Mesh generally needs to be used if setting this to 0 and using a TUN or SLIP interface  
 */
#define UIP_CONF_LLH_LEN 0

/**
 * UDP support on or off (required for DNS)
 * @note DNS support typically requires larger payload sizes (250-300). It seems that DNS servers will typically respond
 * with a single address if requesting an address of www.google.com vs google.com, and this will work with the default payload size
 */
 
#define UIP_CONF_UDP             0
//#define UIP_CONF_BROADCAST       0
//#define UIP_CONF_UDP_CONNS       1

  /***@}*/
  /**
   * @name Advanced Operation
   *
   *  For advanced configuration of RF24Ethernet
   */
  /***@{*/

 /**
  * 
  * Adjust the rate at which the IP stack performs periodic processing.  
  * The periodic timer will be called at a rate of 1 second divided by this value  
  *  
  * Increase this value to reduce response times and increase throughput during user interactions.  
  * @note: Increasing this value will increase throughput for individual nodes, but can impact other network traffic.  
  */
#define UIP_TIMER_DIVISOR 16

/**
 * If operating solely as a server, disable the ability to open TCP connections as a client by setting to 0
 * Saves memory and program space.
 */
 
#define UIP_CONF_ACTIVE_OPEN 1

/**
 * UDP checksums on or off
 */
#define UIP_CONF_UDP_CHECKSUMS   0

/**
* uIP User Output buffer size  
*  
* The output buffer size determines the max 
* length of strings that can be sent by the user, and depends on the uip buffer size  
*  
* Must be <=   UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
* @note Must be an odd number or the TCP/IP sequence gets out of order with payloads larger than 511 bytes
* I think this might be a bug or missing feature of the uip stack
*/
#define UIP_CONF_EXTERNAL_BUFFER

#if UIP_CONF_BUFFER_SIZE >= 512
  #define OUTPUT_BUFFER_SIZE 511
#else
  #define OUTPUT_BUFFER_SIZE UIP_CONF_BUFFER_SIZE - 1 - UIP_CONF_LLH_LEN - UIP_TCPIP_HLEN
#endif

 /**
  * <b>Optional:</b> Used with UIP_CONNECTION_TIMEOUT  
  *
  * If an open connection is not receiving data, the connection will be restarted.  
  * 
  * Adjust the initial delay period before restarting a connection that has already been restarted
  * 
  * For small payloads (96-120 bytes) with a fast connection, this value can be as low as ~750ms or so.
  * When increasing the uip buffer size, this value should be increased, or
  * the window may be reopened while the requested data is still being received, hindering traffic flow.
  */
#define UIP_WINDOW_REOPEN_DELAY  5150

/* @} */ 
/** @} */
/******************** END USER CONFIG ***********************************/

/********** TMRh20: This option is not yet valid **********/
/* for TCP */
#define UIP_SOCKET_NUMPACKETS    1

/**
 * The TCP receive window.
 *
 * This is should not be to set to more than
 * UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN.
 * @note Must be an odd number or the TCP/IP sequence gets out of order with payloads larger than 511 bytes
 * I think this might be a bug or missing feature of the uip stack
 */
 
#if UIP_CONF_BUFFER_SIZE >= 512
  #define UIP_CONF_RECEIVE_WINDOW 511
#else
  //#define UIP_CONF_RECEIVE_WINDOW UIP_CONF_BUFFER_SIZE *2 - UIP_CONF_LLH_LEN - UIP_TCPIP_HLEN //This is set automatically to the max allowable size
  #define UIP_CONF_RECEIVE_WINDOW UIP_CONF_BUFFER_SIZE * 2 - 1 - UIP_CONF_LLH_LEN - UIP_TCPIP_HLEN //This is set automatically to the max allowable size
#endif

#define UIP_CONF_TCP_MSS OUTPUT_BUFFER_SIZE

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
  #if UIP_TIMER_DIVISOR > 5
    #define UIP_CONF_RTO (UIP_TIMER_DIVISOR/2)
  #else
    #define UIP_CONF_RTO 3
  #endif
#endif


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

/** \hideinitializer */
typedef void* uip_tcp_appstate_t;

/** \hideinitializer */
void serialip_appcall(void);
/** \hideinitializer */
#define UIP_APPCALL serialip_appcall

/** \hideinitializer */
typedef void* uip_udp_appstate_t;

/** \hideinitializer */
void uipudp_appcall(void);

/** \hideinitializer */
#define UIP_UDP_APPCALL uipudp_appcall

#if UIP_CONF_LOGGING > 0
void uip_log(char* msg);
#endif

#endif /* __UIP_CONF_H__ */


