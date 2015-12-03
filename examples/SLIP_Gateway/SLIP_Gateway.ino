
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
 * 4. Type 'ls' and look for a new device ttyUSB<X> where <X> is a number
 * 5. Run   sudo modprobe slip
 * 6. Run   sudo slattach -L -s 115200 -p slip /dev/ttyUSB<X> &
 * 7. Note the & at the end of the previous command. Without it, slattach will appear to hang and CTRL+C will exit.
 * 7. Run ifconfig , note the sl<X> device
 * 8. Run   sudo ifconfig sl<X> 10.10.3.1
 * 9. Run   sudo route add -net 10.10.3.0/24 gw 10.10.3.1
 * 10. The gateway is now up and running. Active RF24Ethernet nodes should be pingable.

 Note: If using an ip of 192.168.3.1 for the gateway, the commands are very similar:
        ie: sudo route add -net 192.168.3.0/24 gw 192.168.3.1

 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a sensor node to act as a webserver and
 * allows a user to control a connected LED by clicking links on the webpage
 * The requested URL is used as input, to determine whether to turn the LED off or on
 */

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

#define LED_TXRX          // Flash LED on SLIP device TX or RX 
#define SLIP_DEBUG        // Will delay and flash LEDs if unable to find a node by IP address ( node needs to reconnect via RF24Mesh ) 

// Define the LED pin for the above two options
#define DEBUG_LED_PIN A3

// NOTE: IMPORTANT this should be set to the same value as the UIP_BUFSIZE and
// the MAX_PAYLOAD_SIZE in RF24Network. The default is 120 bytes
#define UIP_BUFFER_SIZE MAX_PAYLOAD_SIZE

uint8_t slip_buf[UIP_BUFFER_SIZE]; // MSS + TCP Header Length

//Function to send incoming network data to the SLIP interface
void networkToSLIP();

void setup() {

  Serial.begin(115200);

  // Set this to the master node (nodeID 0)
  mesh.setNodeID(0);
  mesh.begin();

  // Use the serial port as the SLIP device
  slipdev_init(Serial);

  // LED stuff
  pinMode(DEBUG_LED_PIN, OUTPUT);
#if defined (SLIP_DEBUG)
  digitalWrite(DEBUG_LED_PIN, HIGH);
  delay(200);
  digitalWrite(DEBUG_LED_PIN, LOW);
#endif
}


uint32_t active_timer =0;

void loop() {

  // Provide RF24Network addresses to connecting & reconnecting nodes
  if(millis() > 10000){
    mesh.DHCP();
  }

  //Ensure any incoming user payloads are read from the buffer
  while(network.available()){
    RF24NetworkHeader header;
    network.read(header,0,0);    
  }
  
  // Handle external (TCP) data
  // Note: If not utilizing RF24Network payloads directly, users can edit the RF24Network_config.h file
  // and uncomment #define DISABLE_USER_PAYLOADS. This can save a few hundred bytes of RAM.

  if(mesh.update() == EXTERNAL_DATA_TYPE) {
    networkToSLIP();
  }

  
  // Poll the SLIP device for incoming data
  //uint16_t len = slipdev_poll();
  uint16_t len;
  
  if( (len = slipdev_poll()) > 0 ){
    if(len > MAX_PAYLOAD_SIZE){ return; }
    RF24NetworkHeader header(01, EXTERNAL_DATA_TYPE);    
    uint8_t meshAddr;
    
    // Get the last octet of the destination IP address
    uint8_t lastOctet = slip_buf[19];

    //Convert the IP into an RF24Network Mac address
    if ( (meshAddr = mesh.getAddress(lastOctet)) > 0) {
      // Set the RF24Network address in the header
      header.to_node = meshAddr;
      
      #if defined (LED_TXRX)
        digitalWrite(DEBUG_LED_PIN, HIGH);
      #endif
      
      network.write(header, &slip_buf, len);

      #if defined (LED_TXRX)
        digitalWrite(DEBUG_LED_PIN, LOW);
      #endif
    } else {
      // If nodeID/IP not found in address list, the node would need to renew its address
      // Flash the LED 3 times slowly
      flashLED();
    }

  }

}


void networkToSLIP(){
  
    RF24NetworkFrame *frame = network.frag_ptr;
    size_t size = frame->message_size;
    uint8_t *pointer = frame->message_buffer;
    slipdev_send(pointer, size);
    //digitalWrite(DEBUG_LED_PIN, !digitalRead(DEBUG_LED_PIN));
    
}

void flashLED() {
#if defined (SLIP_DEBUG)
  digitalWrite(DEBUG_LED_PIN, HIGH);
  delay(200);
  digitalWrite(DEBUG_LED_PIN, LOW);
  delay(200);
  digitalWrite(DEBUG_LED_PIN, HIGH);
  delay(200);
  digitalWrite(DEBUG_LED_PIN, LOW);
  delay(200);
  digitalWrite(DEBUG_LED_PIN, HIGH);
  delay(200);
  digitalWrite(DEBUG_LED_PIN, LOW);
  delay(200);
#endif

}
