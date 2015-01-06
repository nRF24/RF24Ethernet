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
 * @warning The buffer size MUST be less than or equal to the MAX_PAYLOAD_SIZE setting in RF24Network_conf.h.
 * 
 * Notes:
 * 1. Nodes can use different buffer sizes, TCP communication is limited to the smallest  
 *  ie: A RPi can be configured to use 1500byte TCP windows, with Arduino nodes using only 120byte TCP windows.
 * 2. Routing nodes handle traffic at the link-layer, so the MAX_PAYLOAD_SIZE is not important, unless they are
 * running RF24Ethernet.  
 * 3. Nodes running RF24Ethernet generally do not need to support RF24Network user payloads. Edit RF24Network_config.h  
 *  and uncomment #define DISABLE_USER_PAYLOADS. This will free memory not used with RF24Ethernet. 
 * 4. The user buffers are automatically configured to (Buffer Size - Link Layer Header Size - TCP Header Size) so
 * using RF24Mesh will decrease payloads by 14 bytes.
 * 5. Larger buffer sizes increase throughput for individual nodes, but can impact other network traffic.
 */

#define UIP_CONF_BUFFER_SIZE     120

 /**
  * 
  * Adjust the rate at which the IP stack performs periodic processing.  
  * The periodic timer will be called at a rate of 1 second divided by this value  
  *  
  * Increase this value to reduce response times and increase throughput during user interactions.  
  * @note: Increasing this value will increase throughput for individual nodes, but can impact other network traffic.  
  */
#define UIP_TIMER_DIVISOR 5


 /**
  * <b>Optional:</b> Uncomment to disable  
  *
  * Adjust the length of time after which an open connection will be timed out.  
  * 
  * If uIP is polling the established connection, but an ack or data is not received for this duration in ms, kill the connection.
  */
#define UIP_CONNECTION_TIMEOUT 20000

 /**
  * <b>Optional:</b> Used with UIP_CONNECTION_TIMEOUT  
  *
  * If an open connection times out, the connection will be restarted.  
  * 
  * Adjust the initial delay period before restarting a connection that has already been restarted
  * 
  * For small payloads (96-120 bytes) with a fast connection, this value can be as low as ~750ms or so.
  * When increasing the uip buffer size, this value should be increased, or
  * the window may be reopened while the requested data is still being received, hindering traffic flow.
  */
#define UIP_WINDOW_REOPEN_DELAY  750

/**
 * SLIP/TUN - 14 for Ethernet/TAP & 0 for TUN/SLIP  
 *  
 * Ethernet headers add an additional 14 bytes to each payload.  
 *  
 * RF24Mesh generally needs to be used if setting this to 0 and using a TUN or SLIP interface  
 */
#define UIP_CONF_LLH_LEN 14



/**
* uIP User Output buffer size  
*  
* The output buffer size determines the max 
* length of strings that can be sent by the user, and depends on the uip buffer size  
*  
* Must be <=   UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
* 
*/
#define OUTPUT_BUFFER_SIZE UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
/* @} */ 
/** @} */
/******************** END USER CONFIG ***********************************/

/********** TMRh20: This option is not yet valid **********/
/* for TCP */
#define UIP_SOCKET_NUMPACKETS    1


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


#endif /* __UIP_CONF_H__ */


