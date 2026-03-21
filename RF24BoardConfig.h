
#ifndef RF24BOARDCONFIG_H
#define RF24BOARDCONFIG_H

/*******************************************************************/

// Initial detection for devices w/CPU >=50MHz
#if (!defined F_CPU || F_CPU >= 50000000)
    // Attempt to use internal stack with non-mbed core for RP2040 or RP2350
    #if (defined ARDUINO_ARCH_RP2040 && !defined ARDUINO_ARCH_MBED) || (defined ARDUINO_ARCH_RP2350 && !defined ARDUINO_ARCH_MBED)
        #ifndef USE_LWIP
            #define USE_LWIP 1
        #endif
        #ifndef RF24ETHERNET_USE_UDP
            #define RF24ETHERNET_USE_UDP 1
        #endif
    #else
        // Add boards here that should be specifically excluded from using lwIP stack (use uIP)
        #if !defined ARDUINO_ARCH_RP2040 && !defined ARDUINO_ARCH_RP2350
            #ifndef USE_LWIP
                #define USE_LWIP 1
            #endif
            #ifndef RF24ETHERNET_USE_UDP
                #define RF24ETHERNET_USE_UDP 1
            #endif
        #endif
    #endif
#endif

/*******************************************************************/

// Set up defines if using lwIP
#if USE_LWIP > 0

    // Add any boards here that use lwIP internally
    #if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_ARCH_ESP8266
        // Use internal IP stack
    #else
        // Use External Arduino lwIP stack
        #define ETHERNET_USING_LWIP_ARDUINO
    #endif

    // Add boards here that require core locking.
    #if defined ARDUINO_ARCH_ESP32
        #if defined CONFIG_LWIP_TCPIP_CORE_LOCKING
            #define RF24ETHERNET_CORE_REQUIRES_LOCKING
            #include <WiFi.h>
            #include "esp_wifi.h"
            #define ETHERNET_APPLY_LOCK  LOCK_TCPIP_CORE
            #define ETHERNET_REMOVE_LOCK UNLOCK_TCPIP_CORE
        #endif
    #endif

    #if (defined ARDUINO_ARCH_RP2040 || defined ARDUINO_ARCH_RP2350) && !defined ARDUINO_ARCH_MBED
        //#define RF24ETHERNET_CORE_REQUIRES_LOCKING
        #include <pico/cyw43_arch.h>
        #define ETHERNET_APPLY_LOCK  cyw43_arch_lwip_begin
        #define ETHERNET_REMOVE_LOCK cyw43_arch_lwip_end
    #endif

    #if defined ARDUINO_ARCH_ESP8266
        #define RF24ETHERNET_CORE_REQUIRES_LOCKING
        #define ETHERNET_APPLY_LOCK()  ets_intr_lock()
        #define ETHERNET_REMOVE_LOCK() ets_intr_unlock()
    #endif

    #include "ethernet_comp.h"
    #include "RF24Client.h"
    #include "RF24Server.h"
    #define HTONS htons
    #if RF24ETHERNET_USE_UDP > 0
        #include "RF24Udp.h"
        #include "Dns.h"
    #endif

    #if !defined ETHERNET_USING_LWIP_ARDUINO
        #include "lwip/ip.h"
        #include "lwip/stats.h"
        #include "lwip/netif.h"
        #include "lwip/snmp.h"
        #include "lwip/timeouts.h"
    #else
        #include <lwIP_Arduino.h>
        #include "lwip/include/lwip/ip.h"
    #endif

#endif

/*******************************************************************/

#endif // BOARDCONFIG_H