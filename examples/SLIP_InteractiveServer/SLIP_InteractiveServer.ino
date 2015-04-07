/*
 * RF24Ethernet Webserver controlling an LED example *
 *
 * **NOTE: This example demonstrates usage the SLIP_Gateway
 *   When using SLIP, there are 3 main differences:
 *    1. The RF24Mesh layer must be used to provide MAC/IP translation
 *    2. The specified RF24Mesh nodeID must be the same as the last octet of the IP
 *        ie: IP: 192.168.1.2  NodeId must be 2
 *
 *    3. The RF24Ethernet library must be configured for TUN
 *       a: Open the uip_conf.h file, make sure to #define UIP_CONF_LLH_LEN 0
 *
 * The following commands must be run on the Linux device to enable slip
 * 1. Connect your Arduino
 * 2. On RPi, cd /dev
 * 3. Type 'ls' and look for a new device ttyUSB<X> where <X> is a number
 * 4. Run   sudo modprobe slip
 * 5. Run   sudo slattach -L -s 115200 -p slip /dev/ttyUSB<X> &
 * 6. Run   sudo ifconfig s<X> 10.10.3.1 dstaddr 10.10.3.2
 * 7. Run   sudo route add -net 10.10.3.0/24 gw 10.10.3.1
 
 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a sensor node to act as a webserver and
 * allows a user to control a connected LED by clicking links on the webpage
 * The requested URL is used as input, to determine whether to turn the LED off or on
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <printf.h>
#include <RF24Ethernet.h>
#include "HTML.h"

// Include RF24Mesh and the EEPROM libs
#include "RF24Mesh.h"
#include "EEPROM.h"

/*** Configure the radio CE & CS pins ***/
RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio, network);

// Create an instance of RF24Mesh
RF24Mesh mesh(radio,network);

#define LED_PIN A3 //Analog pin A3

// Configure the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

/**********************************************************/

void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  printf_begin();
  Serial.println("start");
  pinMode(LED_PIN, OUTPUT);


  // This step is very important. When using TUN or SLIP, the IP of the device
  // must be configured as the NodeID in the RF24Mesh layer
  
  mesh.setNodeID(2);
  mesh.begin();

  //Optional
  radio.printDetails();

  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  
  // NOTE: The last octet/last digit of the IP must match the RF24Mesh nodeID above
  IPAddress myIP(10, 10, 3, 2);
  Ethernet.begin(myIP);

  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10, 10, 3, 1);
  Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.
  server.begin();
}


/********************************************************/

uint32_t mesh_timer = 0;

void loop() {

  // This is the last of the differences between this and the regular Interactive Server example
  // If the master node is completely down, and unresponsive for 30 seconds, renew the address
  if(millis()-mesh_timer > 30000){ //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if( ! mesh.checkConnection() ){
        //refresh the network address        
        mesh.renewAddress();
     }
  }
  
  size_t size;

  if (EthernetClient client = server.available())
  {
    uint8_t pageReq = 0;
    generate_tcp_stats();
    while ((size = client.available()) > 0)
    {
      // If a request is received with enough characters, search for the / character
      if (size >= 7) {
        client.findUntil("/", "/");
        char buf[3] = {"  "};
        buf[0] = client.read();  // Read in the first two characters from the request
        buf[1] = client.read();

        if (strcmp(buf, "ON") == 0) { // If the user requested http://ip-of-node:1000/ON
          led_state = 1;
          pageReq = 1;
          digitalWrite(LED_PIN, led_state);
          
        }else if (strcmp(buf, "OF") == 0) { // If the user requested http://ip-of-node:1000/OF
          led_state = 0;
          pageReq = 1;
          digitalWrite(LED_PIN, led_state);
          
        }else if (strcmp(buf, "ST") == 0) { // If the user requested http://ip-of-node:1000/OF
          pageReq = 2;
          
        }else if (strcmp(buf, "CR") == 0) { // If the user requested http://ip-of-node:1000/OF
          pageReq = 3;
          
        }else if(buf[0] == ' '){
          pageReq = 4; 
        }
      }
      // Empty the rest of the data from the client
      while (client.waitAvailable()) {
        client.read();
      }
    }
    
    /**
    * Based on the incoming URL request, send the correct page to the client
    * see HTML.h
    */
    switch(pageReq){
       case 2: stats_page(client); break;
       case 3: credits_page(client); break;
       case 4: main_page(client); break;
       case 1: main_page(client); break;
       default: break; 
    }    

    client.stop();
    Serial.println(F("********"));

  }

  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

/**
* This section displays some basic connection stats via Serial and demonstrates
* how to interact directly with the uIP TCP/IP stack
* See the uIP documentation for more info
*/
static unsigned short generate_tcp_stats()
{
  struct uip_conn *conn;

  // If multiple connections are enabled, get info for each active connection
  for (uint8_t i = 0; i < UIP_CONF_MAX_CONNECTIONS; i++) {
    conn = &uip_conns[i];

    // If there is an open connection to one of the listening ports, print the info
    // This logic seems to be backwards?
    if (uip_stopped(conn)) {
      Serial.print(F("Connection no "));
      Serial.println(i);
      Serial.print(F("Local Port "));
      Serial.println(htons(conn->lport));
      Serial.print(F("Remote IP/Port "));
      Serial.print(htons(conn->ripaddr[0]) >> 8);
      Serial.print(F("."));
      Serial.print(htons(conn->ripaddr[0]) & 0xff);
      Serial.print(F("."));
      Serial.print(htons(conn->ripaddr[1]) >> 8);
      Serial.print(F("."));
      Serial.print(htons(conn->ripaddr[1]) & 0xff);
      Serial.print(F(":"));
      Serial.println(htons(conn->rport));
      Serial.print(F("Outstanding "));
      Serial.println((uip_outstanding(conn)) ? '*' : ' ');

    }
  }
}

