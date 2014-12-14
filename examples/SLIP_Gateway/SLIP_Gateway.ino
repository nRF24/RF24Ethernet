
/*
 * RF24Ethernet Serial Gateway Example using SLIP *
 * This example will allow RF24Ethernet to be used with any device capable of utilizing the SLIP protocol
 *
 *
 *   When using SLIP, there are 3 main differences:
 *    1. The RF24Mesh layer must be used to provide MAC/IP translation
 *    2. For child nodes (NOT this master/gateway node) The specified RF24Mesh nodeID must be the same as the last octet of the IP
 *        ie: IP: 192.168.1.2  NodeId must be 2
 *
 *    3. The RF24Ethernet library must be configured for TUN
 *       a: Open the uip_conf.h file, set the #define UIP_CONF_LLH_LEN 0
 *
 * The following commands must be run on the Linux device to enable slip
 * 1. On RPi, cd /dev
 * 2. Type 'ls' , note the ttyUSB<X> devices
 * 3. Connect your Arduino
 * 3. Type 'ls' and look for a new device ttyUSB<X> where <X> is a number
 * 4. Run   sudo slattach -L -s 115200 -p slip /dev/ttyUSB<X> &
 * 5. Run ifconfig , note the sl<X> device
 * 6. Run   sudo modprobe slip
 * 6. Run   sudo ifconfig sl<X> 10.10.3.1
 * 7. Run   sudo route add -net 10.10.3.0/24 gw 10.10.3.1
 
 Note: If using an ip of 192.168.3.1 for the gateway, the commands are very similar:
        ie: sudo route add -net 192.168.3.0/24 gw 192.168.3.1
 
 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a sensor node to act as a webserver and
 * allows a user to control a connected LED by clicking links on the webpage
 * The requested URL is used as input, to determine whether to turn the LED off or on
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <RF24Mesh.h>
#include "EEPROM.h"

RF24 radio(7,8);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

#define LED_TXRX          // Flash LED on SLIP device TX or RX 
#define SLIP_DEBUG        // Will delay and flash LEDs if unable to find a node by IP address ( node needs to reconnect via RF24Mesh ) 

// Define the LED pin for the above two options
#define DEBUG_LED_PIN A3

// NOTE: IMPORTANT this should be set to the same value as the UIP_BUFSIZE and 
// the MAX_PAYLOAD_SIZE in RF24Network. The default is 120 bytes
#define UIP_BUFFER_SIZE 120

void setup() {

  Serial.begin(115200);
  
  // Set this to the master node (nodeID 0)
  mesh.setNodeID(0);
  mesh.begin();

  // Use the serial port as the SLIP device
  slipdev_init(Serial);
  
  // LED stuff
  pinMode(DEBUG_LED_PIN,OUTPUT);
  #if defined (SLIP_DEBUG)  
  digitalWrite(DEBUG_LED_PIN,HIGH);
  delay(200);
  digitalWrite(DEBUG_LED_PIN,LOW);
  #endif
}



void loop() {    
    
    // Poll the network and mesh for incoming data or address requests
    uint8_t headerType = mesh.update();
    
    // Provide RF24Network addresses to connecting & reconnecting nodes
    mesh.DHCP();
    
    // Handle external (TCP) data
    // Note: If not utilizing RF24Network payloads directly, users can edit the RF24Network_config.h file
    // and uncomment #define DISABLE_USER_PAYLOADS. This can save a few hundred bytes of RAM.
    
    if(headerType == EXTERNAL_DATA_TYPE){
      RF24NetworkFrame *frame = network.frag_ptr;
      size_t size = frame->message_size;
      uint8_t *pointer = frame->message_buffer;
      slipdev_send(pointer,size);
    }
    
    // Poll the SLIP device for incoming data
      slipdev_poll();

}





