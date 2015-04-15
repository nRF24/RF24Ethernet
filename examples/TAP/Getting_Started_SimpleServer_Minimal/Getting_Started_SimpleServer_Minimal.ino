/*
 * RF24Ethernet Getting_Started_SimpleServer_Minimal example by TMRh20
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to send out an HTTP response to a browser using minimal resources
 *
 * In order to minimize memory use and program space:
 * 1. Open the RF24Network library folder
 * 2. Edit the RF24Networl_config.h file
 * 3. Un-comment #define DISABLE_USER_PAYLOADS
 *
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <RF24Ethernet.h>

/** Configure the radio CE & CS pins **/
RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio,network);

EthernetServer server = EthernetServer(1000);

void setup() {

  uint16_t myRF24NetworkAddress = 02;
  Ethernet.setMac(myRF24NetworkAddress);
  
  IPAddress myIP(10,10,3,7);
  Ethernet.begin(myIP);
  
  IPAddress gwIP(10,10,3,1);
  Ethernet.set_gateway(gwIP);

  server.begin();
}

uint16_t testTimer = 0;

void loop() {

  size_t size;

  if(EthernetClient client = server.available()) {

      while((size = client.waitAvailable()) > 0) {                 
        // Flush any incoming data from the client
        client.flush();
      }
      // Send an HTML response to the client.
      client.write( "HTTP/1.1 200 OK\n Content-Type: text/html\n Connection: close\n Refresh: 5\n");
      client.write( "\n<!DOCTYPE HTML>\n <html>\nHELLO FROM ARDUINO!\n</html>\n");	   
      client.stop();     
  }
 
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}
