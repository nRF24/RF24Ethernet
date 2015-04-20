/*
 * RF24Ethernet Simple Server Example, using RF24Mesh for address allocation
 * RF24toTUN must be built using 'make MESH=1' option
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to send out an HTTP response to a browser
 *
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
//#include <printf.h>
#include <RF24Ethernet.h>
#include "RF24Mesh.h"

/** Configure the radio CE & CS pins **/
RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio,network);
RF24Mesh mesh(radio,network);

// Set up the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  //printf_begin();
  Serial.println(F("start"));
  
  // Note: The mesh nodeID must be equal to the last octet of the assigned
  // IP address
  mesh.setNodeID(4); //Node id must be any unique value between 1 and 250
  mesh.begin();
  //Serial.println(mesh.mesh_address,OCT);
  
  // Set the IP address we'll be using. The last octet of the IP must be equal
  // to the designated mesh nodeID
  IPAddress myIP(10,10,2,4);
  Ethernet.begin(myIP);
  
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10,10,2,2);
  Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.  Each incoming
  // connection will result in the uip_callback() function being called.
  server.begin();
}

uint32_t mesh_timer = 0;

void loop() {

  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if(millis()-mesh_timer > 30000){ //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if( ! mesh.checkConnection() ){
        //refresh the network address        
        mesh.renewAddress();
     }
  }
  
  if(EthernetClient client = server.available())  
  {
     while( client.waitAvailable() > 0){
        Serial.print((char)client.read());
     }
    // Send an HTML response to the client. Default max size/characters per write is 90
    client.write( "HTTP/1.1 200 OK\n Content-Type: text/html\n Connection: close \nRefresh: 5 \n\n");
    client.write( "<!DOCTYPE HTML>\n <html> HELLO FROM ARDUINO!</html>");
    client.stop(); 

    Serial.println(F("********"));       
  }
 
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}
