/** @file RF24Ethernet_config.h*/
 /**
@defgroup DebugOptions
<b>Debugging Options</b>
 @{ */


/** Uncomment <b>#define RF24ETHERNET_DEBUG_CLIENT</b>  to enable main debugging output  
 *  
 * Uncomment <b>#define ETH_DEBUG_L1</b> for debugging window reopening & retransmissions  
 *  
 * Uncomment <b>#define ETH_DEBUG_L2</b> for extra client state debugging  
 *  
 *  
 */
 
 /*********** USER DEBUG CONFIGURATION *********/
//#define RF24ETHERNET_DEBUG_CLIENT 
//#define ETH_DEBUG_L1 
//#define ETH_DEBUG_L2 
/****************************************/

// Note: See uip-conf.h for general user configuration options


  #if defined (RF24ETHERNET_DEBUG_CLIENT)
    #define IF_RF24ETHERNET_DEBUG_CLIENT(x) ({x;})
  #else
    #define IF_RF24ETHERNET_DEBUG_CLIENT(x)
  #endif
  
  #if defined (ETH_DEBUG_L1)
    #define IF_ETH_DEBUG_L1(x) ({x;})
  #else
    #define IF_ETH_DEBUG_L1(x)
  #endif
  
  #if defined (ETH_DEBUG_L2)
    #define IF_ETH_DEBUG_L2(x) ({x;})
  #else
    #define IF_ETH_DEBUG_L2(x)
  #endif
  /* @} */ 
