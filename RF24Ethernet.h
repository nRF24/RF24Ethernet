/*
  RF24Ethernet by TMRh20 2014-2015

  https://github.com/TMRh20

 RF24Ethernet.h - Arduino implementation of a uIP wrapper class.
 Copyright (c) 2014 tmrh20@gmail.com, github.com/TMRh20
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

#ifndef RF24Ethernet_h
#define RF24Ethernet_h

/**
 * @file RF24Ethernet.h
 *
 * Class declaration for RF24Ethernet
 */

#include <Arduino.h>
#if (!defined F_CPU || F_CPU > 50000000)
    #if (defined ARDUINO_ARCH_RP2040 && !defined ARDUINO_ARCH_MBED) || (defined ARDUINO_ARCH_RP2350 && !defined ARDUINO_ARCH_MBED)
        #ifndef USE_LWIP
            #define USE_LWIP 1
        #endif
        #ifndef RF24ETHERNET_USE_UDP
            #define RF24ETHERNET_USE_UDP 1
        #endif
    #else
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

#if USE_LWIP < 1
extern "C" {
    #include "uip-conf.h"
    #include "utility/uip.h"

    #include "utility/uiptimer.h"
    #include "utility/uip_arp.h"
}
#else

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

    #include "ethernet_comp.h"
    #include "RF24Client.h"
    #include "RF24Server.h"
    #define HTONS htons
    #if RF24ETHERNET_USE_UDP > 0
        #include "RF24Udp.h"
        #include "Dns.h"
    #endif

    #if !defined ETHERNET_USING_LWIP_ARDUINO
        #include "lwip\ip.h"
        #include "lwip\stats.h"
        #include "lwip\netif.h"
        #include "lwip\snmp.h"
        #include "lwip\timeouts.h"
    #else
        #include <lwIP_Arduino.h>
        #include "lwip\include\lwip\ip.h"
    #endif

#endif

#include "RF24Ethernet_config.h"
#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_NRF52840) || defined(ARDUINO_ARCH_NRF52833)
    #include <nrf_to_nrf.h>
#endif
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#if !defined(RF24_TAP) // Using RF24Mesh
    #include <RF24Mesh.h>
#endif

#if USE_LWIP < 1
    #include "ethernet_comp.h"
    #include "IPAddress.h"
    #include "RF24Client.h"
    #include "RF24Server.h"

    #if UIP_CONF_UDP > 0 || USE_LWIP > 0
        #include "RF24Udp.h"
        #include "Dns.h"
    #endif

    #define UIPETHERNET_FREEPACKET 1
    #define UIPETHERNET_SENDPACKET 2

    //#define TUN  // Use only the tcp protocol, no ethernet headers or arps
    #define TAP // Include ethernet headers

    #if defined(TAP)
        #define BUF ((struct uip_eth_hdr*)&uip_buf[0])
    #endif
    //#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

    #define uip_seteth_addr(eaddr)          \
        do {                                \
            uip_ethaddr.addr[0] = eaddr[0]; \
            uip_ethaddr.addr[1] = eaddr[1]; \
            uip_ethaddr.addr[2] = eaddr[2]; \
            uip_ethaddr.addr[3] = eaddr[3]; \
            uip_ethaddr.addr[4] = eaddr[4]; \
            uip_ethaddr.addr[5] = eaddr[5]; \
        } while (0)
    #define uip_ip_addr(addr, ip) memcpy(addr, &ip[0], 4)

    #define ip_addr_uip(a) IPAddress(a[0] & 0xFF, a[0] >> 8, a[1] & 0xFF, a[1] >> 8) // TODO this is not IPV6 capable

    #define uip_seteth_addr(eaddr)          \
        do {                                \
            uip_ethaddr.addr[0] = eaddr[0]; \
            uip_ethaddr.addr[1] = eaddr[1]; \
            uip_ethaddr.addr[2] = eaddr[2]; \
            uip_ethaddr.addr[3] = eaddr[3]; \
            uip_ethaddr.addr[4] = eaddr[4]; \
            uip_ethaddr.addr[5] = eaddr[5]; \
        } while (0)

#endif //USE_LWIP < 1

/**
 * @warning <b>This is used internally. Use IPAddress instead. </b>
 */
typedef struct
{
    int a, b, c, d;
} IP_ADDR;

class RF24;
template<class radio_t>
class ESB_Network;

class RF24EthernetClass
{ //: public Print {
public:
/**
 * Constructor to set up the Ethernet layer. Requires the radio and network to be configured by the user
 * this allows users to set custom settings at the radio or network level
 */
#if !defined(RF24_TAP) // Using RF24Mesh
    RF24EthernetClass(RF24& _radio, RF24Network& _network, RF24Mesh& _mesh);
#else
    RF24EthernetClass(RF24& _radio, RF24Network& _network);
#endif
#if defined NRF52_RADIO_LIBRARY
    #if !defined(RF24_TAP)
    RF24EthernetClass(nrf_to_nrf& _radio, RF52Network& _network, RF52Mesh& _mesh);
    #else
    RF24EthernetClass(nrf_to_nrf& _radio, RF52Network& _network);
    #endif
#endif

    /** Basic constructor */
    RF24EthernetClass();

    /**
     * @note Deprecated, maintained for backwards compatibility with old examples
     *
     * This function is no longer needed, and does nothing
     */
    void use_device();

    /**
     * Configure the IP address and subnet mask of the node. This is independent of the RF24Network layer, so the IP
     * and subnet only have to conform to standard IP routing rules within your network
     */
    void begin(IP_ADDR myIP, IP_ADDR subnet);

    /**
     * Configure the IP address and subnet mask of the node. This is independent of the RF24Network layer, so the IP
     * and subnet only have to conform to standard IP routing rules within your network
     */
    void begin(IPAddress ip);
    void begin(IPAddress ip, IPAddress dns);
    void begin(IPAddress ip, IPAddress dns, IPAddress gateway);
    void begin(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);

    /**
     * Configure the gateway IP address. This is generally going to be your master node with RF24Network address 00.
     */
    void set_gateway(IPAddress gwIP);

    /**
     * Listen to a specified port - This will likely be changed to closer match the Arduino Ethernet API with server.begin();
     */
    void listen(uint16_t port);

    /**
     * Sets the MAC address of the RF24 module, which is an RF24Network address
     * Specify an Octal address to assign to this node, which will be used as the Ethernet mac address
     * If setting up only a few nodes, use 01 to 05
     * Please reference the RF24Network documentation for information on setting up a static network
     * RF24Mesh will be integrated to provide this automatically
     */
    void setMac(uint16_t address);

    /** Sets the Radio channel/frequency to use (0-127) */
    void setChannel(uint8_t channel);

    /** Indicates whether data is available. */
    int available();

    /** Returns the local IP address */
    IPAddress localIP();

    /** Returns the subnet mask */
    IPAddress subnetMask();

    /** Returns the gateway IP address */
    IPAddress gatewayIP();

    /** Returns the DNS server IP address */
    IPAddress dnsServerIP();

    /** Keeps the TCP/IP stack running & processing incoming data */
    void update();
    // uint8_t *key;

    uint32_t networkCorruption;

#if !defined NRF52_RADIO_LIBRARY
    RF24Network& network;
    #if !defined(RF24_TAP) // Using RF24Mesh
    RF24Mesh& mesh;
    #endif
#else
    RF52Network& network;
    #if !defined(RF24_TAP) // Using RF24Mesh
    RF52Mesh& mesh;
    #endif
#endif

#if USE_LWIP > 0

    static bool useCoreLocking;
    static constexpr unsigned MAX_FRAME_SIZE = MAX_PAYLOAD_SIZE - 14; // packet size excluding FCS
    static constexpr unsigned MIN_FRAME_SIZE = 60;
    static constexpr unsigned MAX_RX_QUEUE = 5;
    static constexpr uint32_t NetIF_Speed_BPS = 1000000;
    static netif myNetif;

    struct EthQueue
    {
        uint8_t data[MAX_RX_QUEUE][MAX_FRAME_SIZE];
        uint16_t len[MAX_RX_QUEUE];
        uint32_t nRead;
        uint32_t nWrite;
    };
    static EthQueue RXQueue __attribute__((aligned(4)));

    typedef uint32_t err_t;
    static bool isUnicast(const uint8_t frame);
    /** Used internally to initialize incoming data queue */
    static void initRXQueue(EthQueue* RXQueue);
    /** Used internally to write to the internal data queue */
    static void writeRXQueue(EthQueue* RXQueue, const uint8_t* ethFrame, uint16_t lenEthFrame);

private:
    static constexpr uint16_t ETHERNET_MTU = 1500;
    static constexpr uint8_t MacAddr[6] = {0, 1, 2, 3, 4};
    static bool isConnected;

    static pbuf* readRXQueue(EthQueue* RXQueue);

    static void EthRX_Handler(const uint8_t* ethFrame, const uint16_t lenEthFrame);
    static uint8_t networkBuffer[MAX_PAYLOAD_SIZE];
#endif

#if defined NRF52_RADIO_LIBRARY
    nrf_to_nrf& radio;
#else
    RF24& radio;
#endif

#if USE_LWIP < 1
    IPAddress _dnsServerAddress;
#else
    static IPAddress _dnsServerAddress;
#endif

    void configure(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);

    // tick() must be called at regular intervals to process the incoming serial
    // data and issue IP events to the sketch.  It does not return until all IP
    // events have been processed.
    static void tick();
    static void network_send();

    uint8_t RF24_Channel;

#if USE_LWIP < 1
    struct timer periodic_timer;
    #if defined RF24_TAP
    struct timer arp_timer;
    #endif
#endif

    friend class RF24Server;
    friend class RF24Client;
    friend class RF24UDP;
};

extern RF24EthernetClass RF24Ethernet;

typedef RF24EthernetClass RF52EthernetClass;

/**
 * @example Getting_Started_SimpleServer_Mesh.ino
 * <b>Updated: TMRh20 2014 </b><br>
 *
 * This is an example of how to use the RF24Ethernet class to create a web server which will allow you to connect via
 * any device with a web-browser. The url is http://your_chosen_IP:1000
 */

/**
 * @example Getting_Started_SimpleClient_Mesh.ino
 *
 * This is an example of how to use the RF24Ethernet class to connect out to a web server and retrieve data via HTTP.
 */

/**
 * @example Getting_Started_SimpleClient_Mesh_DNS.ino
 *
 * This is an example of how to use the RF24Ethernet class to connect out to a web server and retrieve data via HTTP,
 * using DNS lookups instead of IP address.
 */

/**
 * @example SimpleClient_Mesh.ino
 *
 * This is an example of how to use the RF24Ethernet class to connect out to a web server and retrieve data via HTTP.
 * It also demonstrates how to read from the HTTP header to read the Content-Length before reading the data.
 */

/**
 * @example InteractiveServer_Mesh.ino
 *
 * This is an example of how to create a more advanced interactive web server.<br>
 * This example uses [HTML.h](InteractiveServer__Mesh_2HTML_8h.html) from the
 * example's directory.
 */

/**
 * @example mqtt_basic.ino
 *
 * This is the example taken from the MQTT library https://github.com/256dpi/arduino-mqtt/ & slightly modified to include RF24Ethernet/RF24Mesh.
 */

/**
 * @example mqtt_basic_no_blk.ino
 *
 * This is similar to the mqtt_basic example, but uses a non-blocking connect function.
 */
/**
 * @example mqtt_basic_2.ino
 *
 * A copy of the initial MQTT example using MQTT library https://github.com/256dpi/arduino-mqtt/ slightly modified to include RF24Ethernet/RF24Mesh.
 */

/**
 * @example InteractiveServer_Mesh_Headless.ino
 *
 * This example demonstrates "headless' use of a server, without a gateway device like Raspberry Pi/Linux.
 */

/**
 * @example SimpleClient_Mesh_Headless.ino
 *
 * This example demonstrates "headless" use of a client, without a gateway device like Raspberry Pi/Linux
 */

#endif // RF24Ethernet_h
