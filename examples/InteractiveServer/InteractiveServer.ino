/*
 * RF24Ethernet Webserver controlling an LED example *
 *
 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a sensor node to act as a webserver and
 * allows a user to control a connected LED by clicking links on the webpage
 * The requested URL is used as input, to determine whether to turn the LED off or on
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
//#include <printf.h>
#include <RF24Ethernet.h>
#include <avr/pgmspace.h>
#include "HTML.h"

/*** Configure the radio CE & CS pins ***/
RF24 radio(7, 8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio, network);

#define LED_PIN A3 //Analog pin A3

// Configure the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

/**********************************************************/

void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  //printf_begin();
  Serial.println("start");
  pinMode(LED_PIN, OUTPUT);

  // This initializes the radio with basic settings.
  // Needs to be called at the beginning of every sketch
  Ethernet.use_device();

  // Tell RF24Ethernet which channel to use. This step is not that important, but it
  // is important to set the channel this way, not directly via the radio
  Ethernet.setChannel(97);

  // This step is very important. The address of the node needs to be set both
  // on the radio and in the UIP layer
  // This is the RF24Network address and needs to be configured accordingly if
  // using more than 4 nodes with the master node. Otherwise, 01-04 can be used.
  uint16_t myRF24NetworkAddress = 01;
  Ethernet.setMac(myRF24NetworkAddress);

  //Optional
  radio.setPALevel(RF24_PA_HIGH);
  //radio.printDetails();

  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IPAddress myIP(10, 10, 2, 4);
  Ethernet.begin(myIP);

  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10, 10, 2, 2);
  Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.
  server.begin();
}


/********************************************************/

void loop() {

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
      while (client.available()) {
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

