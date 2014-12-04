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



//********** TMRh20: These options are mostly invalid and should be configured elsewhere **********//
/* for TCP */
#define UIP_SOCKET_NUMPACKETS    1
//#define UIP_CONF_MAX_CONNECTIONS 2

/* for UDP
 * set UIP_CONF_UDP to 0 to disable UDP (saves aprox. 5kb flash) */
//#define UIP_CONF_UDP             0
//#define UIP_CONF_BROADCAST       1
//#define UIP_CONF_UDP_CONNS       4

/* number of attempts on write before returning number of bytes sent so far
 * set to -1 to block until connection is closed by timeout */
//#define UIP_ATTEMPTS_ON_WRITE    -1

/* timeout after which UIPClient::connect gives up. The timeout is specified in seconds.
 * if set to a number <= 0 connect will timeout when uIP does (which might be longer than you expect...) */
//#define UIP_CONNECT_TIMEOUT      -1

/* periodic timer for uip (in ms) */
//#define UIP_PERIODIC_TIMER       250

/* timer to poll client for data after last write (in ms)
 * set to -1 to disable fast polling and rely on periodic only (saves 100 bytes flash) */
//#define UIP_CLIENT_TIMER         10



/************* TMRh20: User Configuration *******************/


/**
 * Maximum number of TCP connections.
 *
 * \hideinitializer
 */
//#define UIP_CONF_MAX_CONNECTIONS 40
//#define UIP_CONF_MAX_CONNECTIONS 12
#define UIP_CONF_MAX_CONNECTIONS 1

/**
 * Maximum number of listening TCP ports.
 *
 * \hideinitializer
 */
//#define UIP_CONF_MAX_LISTENPORTS 40
//#define UIP_CONF_MAX_LISTENPORTS 4
#define UIP_CONF_MAX_LISTENPORTS 1

/**
 * uIP buffer size.
 *
 * \hideinitializer
 */
//#define UIP_CONF_BUFFER_SIZE     420
// ORIG #define UIP_CONF_BUFFER_SIZE     128
#define UIP_CONF_BUFFER_SIZE     120

//#define UIP_REASSEMBLY

/**
 * The TCP maximum segment size.
 *
 * This is should not be to set to more than
 * UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN.
 */
//#define UIP_CONF_RECEIVE_WINDOW 60
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

// SLIP/TUN - 14 for Ethernet/TAP & 0 for TUN
#define UIP_CONF_LLH_LEN 14

//TMRh
//#define UIP_REASSEMBLY

//#define IP_INPUT_BUFFER_LENGTH 16
//#define IP_OUTPUT_BUFFER_LENGTH 16
//#define IP_INPUT_BUFFER_LENGTH 16
//#define IP_OUTPUT_BUFFER_LENGTH 16

typedef void* uip_tcp_appstate_t;

/*
#include "utility/psock.h"
typedef struct serialip_state {
  struct psock p;
  char inputBuffer[IP_INPUT_BUFFER_LENGTH];
  char outputBuffer[IP_INPUT_BUFFER_LENGTH];
  //void *user;
  char packets_in[1];
  char packets_out[1];
  char connected;
} uip_tcp_appstate_t;
*/

void serialip_appcall(void);
#define UIP_APPCALL serialip_appcall

typedef void* uip_udp_appstate_t;
void uipudp_appcall(void);
#define UIP_UDP_APPCALL uipudp_appcall


#endif /* __UIP_CONF_H__ */

/** @} */
