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

extern "C" {
    #include "uip-conf.h"
    #include "utility/uip.h"
    #include "utility/timer.h"
    #include "utility/uip_arp.h"
}

#include "RF24Ethernet_config.h"
#include <RF24.h>
#include <RF24Network.h>
#if !defined (RF24_TAP) // Using RF24Mesh
#include <RF24Mesh.h>
#endif

#include "ethernet_comp.h"
#include "IPAddress.h"
#include "RF24Client.h"
#include "RF24Server.h"

#if UIP_CONF_UDP > 0
#include "RF24Udp.h"
#include "Dns.h"
#endif

#define UIPETHERNET_FREEPACKET 1
#define UIPETHERNET_SENDPACKET 2

//#define TUN  // Use only the tcp protocol, no ethernet headers or arps
#define TAP  // Include ethernet headers

#if defined (TAP)
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#endif
//#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)
#define uip_ip_addr(addr, ip) memcpy(addr, &ip[0], 4)

#define ip_addr_uip(a) IPAddress(a[0] & 0xFF, a[0] >> 8 , a[1] & 0xFF, a[1] >> 8) //TODO this is not IPV6 capable

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)


/**
 * @warning <b>This is used internally. Use IPAddress instead. </b>
 */
typedef struct {
    int a, b, c, d;
} IP_ADDR;

class RF24;
class RF24Network;

class RF24EthernetClass {//: public Print {
public:

    /**
     * Constructor to set up the Ethernet layer. Requires the radio and network to be configured by the user
     * this allows users to set custom settings at the radio or network level
     */
    #if !defined (RF24_TAP) // Using RF24Mesh
    RF24EthernetClass(RF24& _radio, RF24Network& _network, RF24Mesh& _mesh);
    #else
    RF24EthernetClass(RF24& _radio, RF24Network& _network);
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
    //uint8_t *key;

private:
    RF24& radio;
    RF24Network& network;
    #if !defined (RF24_TAP) // Using RF24Mesh
    RF24Mesh& mesh;
    #endif

    static IPAddress _dnsServerAddress;
    void configure(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
    // tick() must be called at regular intervals to process the incoming serial
    // data and issue IP events to the sketch.  It does not return until all IP
    // events have been processed.
    static void tick();
    static void network_send();

    uint8_t RF24_Channel;

    struct timer periodic_timer;
    #if defined RF24_TAP
    struct timer arp_timer;
    #endif
    friend class RF24Server;
    friend class RF24Client;
    friend class RF24UDP;
};

extern RF24EthernetClass RF24Ethernet;

#endif // RF24Ethernet_h

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
 * This is the example taken from the PubSub library (https://github.com/knolleary/pubsubclient) & slightly modified to include RF24Ethernet/RF24Mesh.
 */

/**
 * @example mqtt_basic_2.ino
 *
 * A copy of the initial MQTT example using MQTT library https://github.com/256dpi/arduino-mqtt/ slightly modified to include RF24Ethernet/RF24Mesh.
 */

/**
 * @example SLIP_Gateway.ino
 *
 * This example demonstrates how to use an Arduino as a gateway to a SLIP enabled device.
 */

/**
 * @example SLIP_InteractiveServer.ino
 *
 * This example demonstrates how to use RF24Mesh with RF24Ethernet when working with a SLIP or TUN interface.
 * <br>This example uses [HTML.h](SLIP__InteractiveServer_2HTML_8h.html) from the
 * example's directory.
 */
