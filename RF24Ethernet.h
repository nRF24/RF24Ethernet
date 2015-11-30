/*
  RF24Ethernet by TMRh20 2014-2015
  
  https://github.com/TMRh20

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
  #import "uip-conf.h"
  #import "utility/uip.h"
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
#define uip_ip_addr(addr, ip) do { \
                     ((u16_t *)(addr))[0] = HTONS(((ip[0]) << 8) | (ip[1])); \
                     ((u16_t *)(addr))[1] = HTONS(((ip[2]) << 8) | (ip[3])); \
                  } while(0)

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
		RF24EthernetClass(RF24& _radio,RF24Network& _network, RF24Mesh& _mesh);
		#else
        RF24EthernetClass(RF24& _radio,RF24Network& _network);
        #endif
		/**
		* Basic constructor
		*/
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

		/* *
		* Sets the MAC address of the RF24 module, which is an RF24Network address
		* Specify an Octal address to assign to this node, which will be used as the Ethernet mac address
		* If setting up only a few nodes, use 01 to 05
		* Please reference the RF24Network documentation for information on setting up a static network
		* RF24Mesh will be integrated to provide this automatically
		*/
		void setMac(uint16_t address);
		
		/** Sets the Radio channel/frequency to use (0-127)
		*/
		void setChannel(uint8_t channel);
		

	/** Indicates whether data is available.
	*/
	int available();
	
	/** Returns the local IP address
	*/
	IPAddress localIP();
	/** Returns the subnet mask
	*/
	IPAddress subnetMask();
	/** Returns the gateway IP address
	*/
	IPAddress gatewayIP();
	/** Returns the DNS server IP address
	*/
	IPAddress dnsServerIP();

	/** Keeps the TCP/IP stack running & processing incoming data
	*/
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
		static boolean network_send();		
		
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


#endif



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
 * This is an example of how to create a more advanced interactive web server.
 */
 
  /**
 * @example mqtt_basic.ino
 *
 * This is the example taken from the PubSub library (https://github.com/knolleary/pubsubclient) & slightly modified to include RF24Ethernet/RF24Mesh.
 */
 
 /**
 * @example HTML.h
 *
 * This file is included with the Interactive Server example
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
 */
 
 /**
 * @mainpage RF24Ethernet library: TCP/IP over RF24Network
 * 
 * @section OpenSourceWireless Open & Open-Source IoT Wireless (not WiFi) Networks
 * **An experiment disconnected...**
 * <img src="tmrh20/RF24EthernetOverviewImg.jpg" alt="RF24EthernetImage" style="width:70%;height:70%">
 * 
 * @section Goals Goals:
 * 
 * - Bring the reliability of TCP/IP and potential of mesh networking/IoT to even the smallest of Arduino devices
 * - Enable self-sustaining wireless sensor networks that seamlessly link together using standard protocols & networking
 * - Simplify & automate it.
 * - Experiment with/document a model for completely open & open-source communication
 *
 * 
 * @section News Update News
 * 
 * \version <b>1.6 - Aug-Dec 2015</b>
 * - Address problems with stream functions like client.parseInt() or find()
 * - Tested working with MQTT via <a href="https://github.com/knolleary/pubsubclient">PubSub library</a>
 * - Fix: Connection state set before begin allocated
 * - Workaround for HTTP servers sending half TCP MSS
 * - Automatically assign mesh nodeID based on IP & update examples
 * - <a href="https://github.com/esp8266/Arduino">ESP8266 (Arduino)</a> support
 *
 * \version <b>1.51RC1 - Apr15-16 2015</b>
 * - Seemingly stable release candidate
 * - Major change: <a href="http://tmrh20.github.io/RF24Gateway/">RF24Gateway</a> replaces RF24toTUN
 * - Now defaults to using RF24Mesh (TUN) interface 
 * - Apr 16 - Use external buffer for uIP to save memory. Requires Updating RF24Network and RF24Mesh.
 * 
 * See <a href="VersionInfo.html"> version history </a> for more info 
 * 
 * @section Config Configuration and Setup
 * 
 * The hardest part of setting up is getting the first radio module connected properly. <br>
 * Scroll to the bottom of the RF24 <a href="http://tmrh20.github.io/RF24/index.html"> radio documentation for pin connections </a> <br><br>
 * Once you have done that, see the <a href="ConfigAndSetup.html">Configuration and Set-Up</a> page for general installation and configuration information
 *  <br><br> 
 *
 * @section DetailOverview Detailed Overview
 * See the <b><a href="Overview.html">Overview</a></b> page
 * 
 * <br>
 * @section BuildingANetwork Building a Network - Customizing your RF24 TCP/IP network
 * 
 * See the <a href="CustomNetworks.html">Building a network</a> page
 *  <br>  <br> 
 * @section Troubleshooting Troubleshooting
 *
 * The RF24 libraries are divided into layers, generally according to the OSI model, which allows specialized testing and troubleshooting of individual layers. 
 * <br>See the <a href=Troubleshooting.html >Troubleshooting</a> section for general troubleshooting steps.
 *  <br>  <br> 
 * @section AdditionalInfo Additional Information
 * 

 *
 *
 *
 *
 *
 *
 * 
 *
 * @page Overview RF24Ethernet Overview
 * @section Overview Overview
 *
 * The RF24Ethernet library was initially designed as an experiment and potential testing tool for <a href = http://tmrh20.github.io/RF24Network_Dev/RF24Network > RF24Network </a>, an OSI Layer 3 network driver, allowing a Raspberry Pi to
 * act as a TCP/IP gateway or host for connected sensor nodes. An Arduino can interface with any Linux machine or SLIP capable device supporting USB, or
 * preferably, an RPi runs companion software, <a href="http://tmrh20.github.io/RF24Gateway/">RF24Gateway</a>, which creates a network interface linked to the RF24 radio network. This interface can be
 * further linked to the local network or internet. This allows the RPi or Arduino-based gateway to perform automatic discovery and routing of TCP/IP data,
 * with no required pre-configuration or interaction from the user beyond assigning appropriate addresses to the nodes initially.
 * 
 * @section What What does it do?
 *
 * RF24Ethernet creates a network of internet enabled RF24/Arduino sensors. It provides an API and interface very similar to the Arduino Ethernet library,
 * to allow sensor nodes to connect out to local or internet based devices to retrieve or send information, or be connected to from the internet or your
 * local network directly.  
 * 
 * Sensor nodes can act as individual web servers, simple command-and-control servers, or can connect out as required via TCP/IP. 
 *
 *
 * @section How How does it work?
 *
 * RF24Ethernet utilizes the UIP TCP/IP stack, allowing Arduino devices to use a Raspberry Pi running <a href="http://tmrh20.github.io/RF24Gateway/">RF24Gateway</a> or Arduino
 * as a gateway to your network or the internet, or simply as a repository for sensor information. The RF24, RF24Network and optionally RF24Mesh libraries
 * handle the underlying routing, addressing etc. so users do not need to be familiar with the radio modules or libraries.  
 *  
 * RF24Network addresses need to be assigned as MAC addresses, and IP addresses can be configured as desired by the user. The master node (00) uses
 * either the Address Resolution Protocol (ARP) or RF24Mesh to find the appropriate nodes when IP traffic is sent though, and routes traffic to the correct
 * RF24Network address.
 * 
 * This provides a fairly seamless interaction, since users only need to configure standard IP forwarding and firewall rules as desired.
 *
 * The RF24 libraries are based on the <a href = http://en.wikipedia.org/wiki/OSI_model > OSI model</a>:<br>
 * | System                 | OSI Layer              | Description |
 * |------------------------|------------------------|---------------------------------------------------------------------------|
 * | <b>NRF24L01 Radio Modules</b> | OSI Layer 1 (Physical) | Transmission and reception of the raw bit stream over the physical medium. |
 * | <b>RF24 Core Radio Driver</b> | OSI Layer 2 (Data Link)| Transfer of data frames over the physical link. Establish/Terminate logical links between nodes. Manage acknowledgements, error checking |
 * | <b>RF24Network</b>            | OSI Layer 3 (Network)  | Routing, subnets, subnet traffic control, logical-to-physical address mapping, frame fragmentation/reassembly.|
 * | <b>RF24Ethernet (uIP)</b>           | OSI Layer 4 (Transport)| Ensures error-free messages. Manages sequencing, losses, retries, and duplicates. |
 * | <b>RF24Ethernet (core)</b>          | OSI Layer 5 (Session)  | Establish, maintain, and terminate connections |
 * | <b>RF24Mesh</b> -Optional-     | OSI Layer 7 (Application)| Provides DHCP/DNS type protocols and allows dynamic addressing/topology for RF24Network. 
 *
 *
 *
 * **TAP vs TUN:**
 *
 * RF24Ethernet and RF24Gateway are able to utilize both TAP and TUN type interfaces. <br>
 * <b> TAP -</b> A TAP interface can be looked at much the same as an Ethernet interface, Ethernet headers are used to identify devices via MAC address, and the Address
 * Resolution Protocol (ARP) is used to perform that identification. <br>
 * <b> TUN -</b> A TUN interface does not utilize Ethernet headers, MAC addresses or ARP, in this case relying on RF24Mesh/IP routing instead.
 *
 * **Limitations:**
 *
 * UDP frames can be up to 512 bytes in length, so UDP/DNS is limited by available memory, and the defined MAX_PAYLOAD_SIZE / UIP_BUFFER_SIZE. If a 
 * DNS request exceeds the maximum allowed size, it will be dropped. DNS responses for addresses such as 'www.google.com' tend to be much smaller than requests
 * for 'google.com', and will generally work with the default configuration.
 *
 * **MAC address formatting:**
 *
 * RF24Ethernet uses a simple method of formatting the MAC addresses, using the first four bytes to store the characters 'RF24', and the last two bytes to store the
 * RF24Network address, which makes identification of nodes via MAC address very simple using standard monitoring tools.
 *
 * | RF24Network Address| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 |
 * |--------------------|--------|--------|--------|--------|--------|--------|
 * |      n/a           |   R    |   F    |   2    |   4    |Octal LB|Octal HB|
 * |      01            |  0x52  |  0x46  |  0x32  |   0x34 |  0x01  |  0x00  |
 * |      011           |  0x52  |  0x46  |  0x32  |   0x34 |  0x09  |  0x00  |
 * |      0443          |  0x52  |  0x46  |  0x32  |   0x34 |  0x23  |  0x01  |
 *
 * <br><br>
 *
 *
 *
 *
 * 
 * @page ConfigAndSetup Configuration and Set-Up
 *
 * RF24Ethernet requires the RF24 and RF24Network_DEV libraries (optionally RF24Mesh) <br>
 * See http://tmrh20.github.io for documentation and downloads <br>
 *
 * See the video at https://www.youtube.com/watch?v=rBAIqAaRu0g for a walk-through of the software setup with Raspberry Pi and Arduino.
 *  <br><br>
 *  <b> RPi </b>
 *
 * On the Raspberry Pi, a companion program, <a href="http://tmrh20.github.io/RF24Gateway/">RF24Gateway</a> must be installed along with the RF24 and RF24Network libraries
 * 1. @code wget http://tmrh20.github.io/RF24Installer/RPi/install.sh  @endcode  
 * 2. @code chmod +x install.sh  @endcode  
 * 3. @code ./install.sh  @endcode  
 * 4. @code cd RF24Gateway/examples/ncurses @endcode
 * 5. @code make @endcode
 * 6. @code sudo ./RF24Gateway_ncurses @endcode  
 * 7. The application will require the user to specify an IP address and Subnet Mask: 10.10.2.2 and 255.255.255.0 are the defaults with RF24Ethernet examples 
 * 8. Raspberry Pi defaults to the master node (00) using RF24Mesh. Secondary Raspberry pi nodes need to specify their RF24Network address or RF24Mesh nodeID.
 *   
 *<b> Arduino </b>
 * 1. For Arduino devices, use the Arduino Library Manager to install the RF24 libraries
 * 2. Open the included Getting_Started_SimpleServer or Getting_Started_SimpleClient example
 * 3. Configure your chosen CE and CS pins for the radio connection.
 * 4. Configure the RF24Mesh nodeID (Any unique value from 3 to 255)
 * 5. Configure the IP address according to your preferences, (last octet must==nodeID) with the gateway set to the chosen IP of the RPi.
 * 6. Connect into your nodes web-server at http://ip-of-your-node:1000 from the RPi or configure the client sketch to connect to a server
 * running on the Raspberry Pi.
 * Note: To minimize memory usage on Arduino, edit RF24Network_config.h with a text editor, and uncomment #define DISABLE_USER_PAYLOADS. This
 * will disable standard RF24Network messages, and only allow external data, such as TCP/IP information. 
 *
 * <b> Non-Raspberry Pi (Linux etc) Devices </b><br>
 * Arduino can also function as a gateway for any Linux machine or PC/MAC that supports SLIP. <br>
 * See the SLIP_Gateway and SLIP_InteractiveServer
 * examples for usage without the need for a Raspberry Pi.
 * 
 * <b>Accessing External Systems: Forwarding and Routing</b>
 *
 * In order to give your network or nodes access to your network or the internet beyond the RPi, it needs to be configured to route traffic
 * between the networks.
 * 1. Run @code sudo sysctl -w net.ipv4.ip_forward=1 @endcode to allow the RPi to forward requests between the network interfaces
 * 2. @code sudo iptables -t nat -A POSTROUTING -j MASQUERADE @endcode to allow the RPi to perform NAT between the network interfaces <br>
 *
 * @note This configuration is generally for initial testing only. Users may also need to add a static route to their local machine, or configure port forwarding on the RPi.<br>
 * See the following links for more information on configuring and using IPTables:<br>
 * http://www.karlrupp.net/en/computer/nat_tutorial<br>
 * http://serverfault.com/questions/326493/basic-iptables-nat-port-forwarding
 *   
 * @warning <b>Note:</b> Users are responsible to manage further routing rules along with their IP traffic in order to prevent unauthorized access. 
 * 
 * @section Advanced Advanced (uIP) Configuration and Info
 *
 * See the <a href="group__UipConfiguration.html">User Configuration</a> section for modifying uip-conf.h
 *
 *
 *
 * @page CustomNetworks Building an RF24Ethernet Network
 * 
 * 
 * The general configuration instructions set up a basic sensor network, with static nodes, using TCP/IP over RF24Network. The layered 
 * library design also allows some nodes to communicate solely at the RF24Network level, creating hybrid networks to extend range over
 * large distances, or improve connectivity in distant areas.
 *
 *
 * @section RF24Mesh RF24Mesh Networks
 * 
 * RF24Mesh allows dynamic configuration of RF24Network addresses, and allows the creation of dynamic sensor networks. Nodes are able
 * to move around physically, releasing and renewing their RF24Network address as required. Once configured with a unique nodeID, nodes using RF24Mesh
 * can automatically configure themselves and join the network as required, with no user interaction.
 *
 * See the included <a href = SimpleServer_mesh_8ino-example.html>SimpleServer_Mesh</a> example for general usage and configuration information
 * with RF24Ethernet.
 *
 * **RF24Mesh** - Additional Information
 *
 * When utilizing RF24Mesh along with RF24Ethernet, relay/routing nodes should be configured with RF24Mesh, or designated on the master node as static nodes
 * with a pre-configured RF24Network address.
 *
 * RF24Mesh can be used with TAP/Ethernet configurations, but utilizing TUN/RF24Mesh together will reduce overhead.
 * 
 * See the links below for more information on utilizing RF24Mesh with RF24Ethernet and RF24Network.
 *
 * @section RF24Gateway RF24Gateway
 * 
 * See http://tmrh20.github.io/RF24Gateway/ for more information on RF24Gateway
 *
 ** @section HybridNetworks Hybrid- RF24Network/RF24Ethernet Networks
 * 
 * The default configuration of RF24Ethernet and RF24Network enables both TCP/IP and the underlying messaging protocols provided by RF24Network.
 *
 * Relay/Routing nodes can be configured using only the RF24 and RF24Network layers (optionally RF24Mesh), and will automatically handle data
 * accordingly, whether it is an external data type (TCP/IP) or an internal network message from another RF24Network node. This allows very small and
 * inexpensive devices such as ATTiny to act as relay/routing nodes when extending network range and/or connectivity.
 *
 * Arduino/AVR devices can utilize custom or included sleep modes in RF24Network, and benefit from the lower power consumption and low-power features
 * of the nrf24l01 chips directly.  
 * 
 * @section Links Links
 *
 * See http://tmrh20.github.io/RF24Mesh for more information on using RF24Mesh to create a dynamic network, with automatic RF24Network addressing <br>
 * See http://tmrh20.github.io/RF24Network_Dev for more information on using RF24Network as well as addressing and topology.
 * <br><br><br>
 *
 *
 *
 *
 *
 * @page Troubleshooting Troubleshooting
 *
 * @section GeneralTrouble General Troubleshooting
 *  <br>
 * @section Compilation Installation & Compilation Errors:
 * **RPi:** <br> Report issues with the installer at https://github.com/TMRh20/RF24/issues  
 * 
 * **Arduino:**
 * 1. Ensure proper library installation. <br>
 *  a: Download and install RF24, RF24Network, RF24Mesh and RF24Ethernet libraries from http://tmrh20.github.io <br>
 *  b: See http://arduino.cc/en/Guide/Libraries ( Note: Library folders may need to be renamed ie: Change RF24Network-Development to RF24Network ) <br>
 *  
 *  <br>
 * @section HardwareConfig Test hardware configuration
 * **RPi:**<br>
 *  a: Navigate to the rf24libs/RF24/examples_RPi folder<br>
 *  b: Configure the correct pins in gettingstarted.cpp ( See http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939 ) <br>
 *  c: Build the examples: @code make @endcode 
 *  d: Run an example @code sudo ./gettingstarted @endcode
 *
 * **Arduino:** <br>
 *  a: Open the GettingStarted example sketch (File > Examples > RF24 > GettingStarted) <br>
 *  b: Configure the appropriate CE and CS pins in the example as desired ( RF24 radio(7,8); ) <br>
 *  c: Configure the assigned radio number in the example <br>
 *  d: Upload to Arduino to test <br>
 *
 *  <br>
 * @section TestingTCP Testing RF24Gateway and RF24Ethernet
 * @note Troubleshooting steps assume a fresh library install with the default configuration, using RF24Mesh/TUN
 * @warning The maximum payload size configured in RF24Network_config.h will determine the maximum size of TCP or UDP segments. Set to 1514 (TAP) or 1500 (TUN) on Raspberry Pi/Linux devices for full TCP/IP capabilities. TCP+IP+LL headers add 54 bytes of overhead to each payload with TAP/Ethernet, and 40 bytes with TUN/RF24Mesh
 *  
 * **RPi (<a href="http://tmrh20.github.io/RF24Gateway/">RF24Gateway</a>):** <br>
 *  a: Run the included RF24Gateway_ncurses example @code sudo ./RF24Gateway_ncurses @endcode 
 *  b: Test connectivity: @code ping 10.10.3.<IP_TO_PING-last-octet> @endcode 
 *  c: If connectivity fails, ensure IP information is accurate. Forwarding is required if pinging from a device other than the RPi. <br>
 *  d: If using pins connections other than the default, edit the RF24 constructor in the example file: ie: RF24 radio(22,0); <br>
 *  e: Optional: For RPi to RPi communication over RF24, edit the rf24libs/RF24Network/RF24Network_config.h file. Set @code #define MAX_PAYLOAD_SIZE 1514 @endcode
 *
 * **Arduino (RF24Ethernet):** <br>
 *  a: Update all RF24 libraries (RF24, RF24Network, RF24Mesh, RF24Ethernet) http://tmrh20.github.io <br>
 *  b: Open, configure and upload one of the included examples to Arduino. ( File > Examples > RF24Ethernet ) <br>
 *  c: Ensure example hardware and pin configuration matches CE and CS pins selected in step 2 <br>
 *  d: If switching between TAP(Ethernet) & TUN(RF24Mesh), ensure the LLHL is defined appropriately in RF24Ethernet/uip-conf.h: @code #define UIP_CONF_LLH_LEN 14 @endcode
 *
 *  
 *  
 *<br><br><br> 
 *
 *
 *
 * @page VersionInfo Version Info
 *
 * \version <b>1.5RC1 - Apr15-16 2015</b>
 * - Seemingly stable release candidate
 * - Major change: <a href="http://tmrh20.github.io/RF24Gateway/">RF24Gateway</a> replaces RF24toTUN
 * - Now defaults to using RF24Mesh (TUN) interface 
 * - Apr 16 - Use external buffer for uIP to save memory. Requires Updating RF24Network and RF24Mesh. 
 *
 * \version <b>1.4b/1.411b - March 14 - Apr 7 2015 </b>
 * - Add Ethernet.update() function
 * - Improve/Fix outgoing data handling
 * - Fix: Hanging in 1.4b
 *
 * \version <b>1.3b - March 6 2015</b>
 * - Major Improvement: Better TCP window handling increases reliability, throughput and performance
 *
 * \version <b>1.24b - March 3 2015</b>
 * - Utilize separate incoming/outgoing buffers (bugfix)
 * - Update documentation for DNS & UDP
 * - Add waitAvailable() function, update examples to demonstrate usage.
 *
 * \version  <b>1.23b - Jan 22 2015</b>
 *  - Small bugfixes from v1.20
 *  - Slightly reduced latency
 *  - Code clean-up/Reduce code size and memory usage for main Client/Server code
 *  - Cleaned up some examples, added DNS and SimpleServer_Minimal examples
 *  
 * \version  <b>1.221b - Jan 16 2015</b>
 *  - Add UDP support
 *  - Add DNS support
 *  - Add support for up to 512 byte buffer size
 *  - Reduce used memory and program space
 *  - Support for multiple connections with per-connection memory  buffers
 *
 * \version  <b>1.1b - Jan 4 2015</b>  
 *  - Add connection timeout to recover from hangs during failed client downloads
 *  - Better TCP window management to prevent hangs during client downloads
 *  - Stability improvements
 *
 * \version  <b>1.0b - Dec 2014</b>
 *  - Outgoing client data corruption should be fixed
 */

 
 
 
 