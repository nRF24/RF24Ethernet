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
		RF24EthernetClass(RF24& _radio,RF24Network& _network);
		
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
	static uint8_t packetstate;
	static uint8_t uip_hdrlen;
	
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

	//uint8_t myData[OUTPUT_BUFFER_SIZE];
	uint32_t lastRadio;

	private:
		RF24& radio;
		RF24Network& network;
		uint8_t in_packet;
		
		static IPAddress _dnsServerAddress;
		void configure(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
		// tick() must be called at regular intervals to process the incoming serial
		// data and issue IP events to the sketch.  It does not return until all IP
		// events have been processed.
		static void tick();
		static boolean network_send();		
		
		uint8_t RF24_Channel;
		int handle_connection(uip_tcp_appstate_t *s);
		struct timer periodic_timer;
		
		struct timer arp_timer;
		void uip_callback();

	//friend void serialip_appcall(void);
	//friend void uipudp_appcall(void);
    
	friend class RF24Server;
    friend class RF24Client;
	friend class RF24UDP;
};

//void handle_ip_event(uint8_t type, ip_connection_t *conn, void **user);

extern RF24EthernetClass RF24Ethernet;


#endif



/**
 * @example Getting_Started_SimpleServer.ino
 * <b>Updated: TMRh20 2014 </b><br>
 *
 * This is an example of how to use the RF24Ethernet class to create a web server which will allow you to connect via
 * any device with a web-browser. The url is http://your_chosen_IP:1000
 */
  
/**
 * @example Getting_Started_SimpleClient.ino
 * <b>Updated: TMRh20 2014 </b><br>
 *
 * This is an example of how to use the RF24Ethernet class to connect out to a web server and retrieve data via HTTP.
 */
 
 /**
 * @example Getting_Started_InteractiveServer.ino
 *
 * This is an example of how to create a simple interactive web server.
 */
 
 /**
 * @example InteractiveServer.ino
 *
 * This is an example of how to create a more advanced interactive web server.
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
 * @section Overview Overview
 *
 * The RF24Ethernet library was initially designed as an experiment and potential testing tool for RF24Network, allowing a Raspberry Pi to
 * act as a TCP/IP gateway or host for connected sensor nodes. An Arduino can interface with any Linux machine or SLIP capable device supporting USB, or
 * preferably, an RPi runs companion software, RF24toTUN, which creates a network interface linked to the RF24 radio network. This interface can be
 * further linked to the local network or internet. This allows the RPi or Arduino gateway to perform automatic discovery and routing of TCP/IP data,
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
 * RF24Ethernet utilizes the UIP TCP/IP stack, allowing Arduino devices to use a Raspberry Pi running RF24toTUN or Arduino
 * as a gateway to your network or the internet, or simply as a repository for sensor information. The RF24, RF24Network and optionally RF24Mesh libraries
 * handle the underlying routing, addressing etc. so users do not need to be familiar with the radio modules or libraries.  
 *  
 * RF24Network addresses need to be assigned as MAC addresses, and IP addresses can be configured as desired by the user. The master node (00) uses
 * either the Address Resolution Protocol (ARP) or RF24Mesh to find the appropriate nodes when IP traffic is sent though, and routes traffic to the correct
 * RF24Network address.
 * 
 * This provides a fairly seamless interaction, since users only need to configure standard IP forwarding and firewall rules as desired.
 *
 * @section News Update News
 * 
 * \version  <b>1.21b - Jan 16 2015</b>
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
 *
 * @section Config Configuration and Setup
 * 
 * RF24Ethernet requires the RF24 and RF24Network_DEV libraries (optionally RF24Mesh) found on my github repository at https://github.com/TMRh20  <br>
 *  <br>
 *  <b> RPi </b>
 * On the Raspberry Pi, a companion program, RF24toTUN must be installed along with the RF24 and RF24Network libraries
 * @note RF24toTUN requires the boost libraries. Run @code sudo apt-get install libboost1.50-all @endcode
 * 
 * See the video at https://www.youtube.com/watch?v=rBAIqAaRu0g for a walk-through of the setup with Raspberry Pi and Arduino.
 *
 * 1. @code wget http://tmrh20.github.io/RF24Installer/RPi/install.sh  @endcode  
 * 2. @code chmod +x install.sh  @endcode  
 * 3. @code ./install.sh  @endcode  
 * 4. Edit the included rf24totun_configAndPing.sh to modify the IP information to suit your desired network config  
 * 5. @code sudo ./rf24totun_configAndPing.sh 2 3 @endcode  
 * 6. The '2' specifies the last octet of the RPi IP address. The '3' specifies the last octet of the node it will attempt to ping once started.  
 * 7. Raspberry Pi defaults to the master node (00). Secondary Raspberry pi nodes would need the MAC address modified accordingly.
 *   
 *<b> Arduino </b>
 * 1. For Arduino devices, the RF24, RF24Network and RF24Ethernet libraries need to be installed.
 * 2. Open the included Getting_Started_SimpleServer or Getting_Started_SimpleClient example
 * 3. Configure the RF24Network address (see http://tmrh20.github.io/RF24Network_Dev/ for more info)
 * 4. Configure the IP address according to your preferences, with the gateway set to the chosen IP of the RPi.
 * 5. Connect into your nodes web-server at http://ip-of-your-node:1000 from the RPi or configure the client sketch to connect to a server
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
 * @warning <b>Note:</b> Users are responsible to manage further routing rules along with their IP traffic in order to prevent unauthorized access. 
 * Users may also need to add a static route or port forwarding to their PC or router to connect in to or ping the sensor nodes.
 * @section Advanced Advanced Configuration and Info
 *
 * See the <a href="group__UipConfiguration.html">User Configuration</a> section for modifying uip-conf.h
 *
 * @section Limitations Limitations
 *
 * The library is currently limited to TCP communications only, with statically assigned IP addresses. UDP frames can be up to 512 bytes in length, so additional
 * functionality (DHCP/DNS) is limited by available memory. At this time, there are no immediate plans to develop UDP functionality.
 *
 * @section MAC_Addys MAC address formatting:
 * RF24Ethernet uses a simple method of formatting the MAC addresses, using the first two bytes to store the RF24Network address, and the remaining
 * four bytes to store the characters 'RF24', which makes identification of nodes via MAC address very simple using standard monitoring tools.
 *
 * | RF24Network Address| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 |
 * |--------------------|--------|--------|--------|--------|--------|--------|
 * |      n/a           |   R    |   F    |   2    |   4    |Octal LB|Octal HB|
 * |      01            |  0x52  |  0x46  |  0x32  |   0x34 |  0x01  |  0x00  |
 * |      011           |  0x52  |  0x46  |  0x32  |   0x34 |  0x09  |  0x00  |
 * |      0443          |  0x52  |  0x46  |  0x32  |   0x34 |  0x23  |  0x01  |
 *
 */

 
 
 
 