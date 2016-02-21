/*
 RF24Server.cpp - Arduino implementation of a uIP wrapper class.
 Copyright (c) 2014 tmrh20@gmail.com, github.com/TMRh20 
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
  
/** @file RF24Ethernet_config.h*/
 /**
@defgroup DebugOptions
<b>Debugging Options</b>
 @{ */


/*********** USER DEBUG CONFIGURATION *********/
//#define RF24ETHERNET_DEBUG_CLIENT 
//#define ETH_DEBUG_L1 
//#define ETH_DEBUG_L2 
//#define RF24ETHERNET_DEBUG_UDP
//#define RF24ETHERNET_DEBUG_DNS
/****************************************/

// Note: See uip-conf.h for general user configuration options

/** 
 * Uncomment <b>#define RF24ETHERNET_DEBUG_CLIENT</b>  to enable main debugging output  
 */
  #if defined (RF24ETHERNET_DEBUG_CLIENT)
    #define IF_RF24ETHERNET_DEBUG_CLIENT(x) ({x;})
  #else
    #define IF_RF24ETHERNET_DEBUG_CLIENT(x)
  #endif
 /** 
  * Uncomment <b>#define ETH_DEBUG_L1</b> for debugging window reopening & retransmissions  
  */
  #if defined (ETH_DEBUG_L1)
    #define IF_ETH_DEBUG_L1(x) ({x;})
  #else
    #define IF_ETH_DEBUG_L1(x)
  #endif
 /**
  * Uncomment <b>#define ETH_DEBUG_L2</b> for extra client state debugging  
  */  
  #if defined (ETH_DEBUG_L2)
    #define IF_ETH_DEBUG_L2(x) ({x;})
  #else
    #define IF_ETH_DEBUG_L2(x)
  #endif
  /**
   * Uncomment <b>#define RF24ETHERNET_DEBUG_UDP</b> for UDP debugging  
   */
  #if defined (RF24ETHERNET_DEBUG_UDP)
    #define IF_RF24ETHERNET_DEBUG_UDP(x) ({x;})
  #else
    #define IF_RF24ETHERNET_DEBUG_UDP(x)
  #endif
  /**
   * Uncomment <b>#define RF24ETHERNET_DEBUG_DNS</b> for DNS debugging  
   */
  #if defined (RF24ETHERNET_DEBUG_DNS)
    #define IF_RF24ETHERNET_DEBUG_DNS(x) ({x;})
  #else
    #define IF_RF24ETHERNET_DEBUG_DNS(x)
  #endif
  
  /* @} */ 
