/*
 * *************************************************************************
 * RF24Ethernet Arduino library by TMRh20 - 2014-2015
 * 
 * Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules
 * 
 * RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet 
 *                     -> RF24Mesh
 *                     
 *      Documentation: http://tmrh20.github.io/RF24Ethernet/
 *      
 * *************************************************************************             
 * 
 * What it does?: 
 * 
 * RF24Ethernet allows tiny Arduino-based sensors to automatically 
 * form and maintain an interconnected, wireless mesh network capable of utilizing
 * standard (TCP/IP) protocols for communication. ( Nodes can also use
 * the underlying RF24Network/RF24Mesh layers for internal communication. )
 * 
 * Any device with a browser can connect to and control various sensors, and/or the sensors
 * can communicate directly with any number of IP based systems.
 * 
 * Why? 
 * 
 * Enabling TCP/IP directly on the sensors enables users to connect directly
 * to the sensor nodes with any standard browser, http capable tools, or with
 * virtually any related protocol. Nodes are able to handle low level communications
 * at the network layer and/or TCP/IP based connections. 
 * 
 * Remote networks can be easily interconnected using SSH tunnelling, VPNs etc., and 
 * sensor nodes can be configured to communicate without the need for an intermediary or additional programming.
 * 
 * Main Features:
 * 
 * 1. Same basic feature set as any Arduino Ethernet adapter, only wireless...
 * 2. Uses RPi OR Arduino+Linux OR Arduino + any SLIP capable device as the wireless gateway/router.
 * 3. Easy Arduino configuration: Just assign a unique IP address to each node, ending in 2-255 (ie: 192.168.1.32)
 *    *Linux devices use standard TCP/IP networking (IPTABLES,NAT,etc) and tools (wget, ftp, curl, python...)
 * 4. Automated (mesh) networking creates and maintains network connectivity as nodes join the network or move around
 * 5. Automated, multi-hop routing allows users to greatly extend the range of RF24 devices
 * 6. API based on the official Arduino Ethernet library. ( https://www.arduino.cc/en/Reference/Ethernet )
 * 7. RF24Gateway (companion program for RPi) provides a user interface that automatically handles TCP/IP
 *    data, and is easily modified to handle custom RF24Network/RF24Mesh data.
 * 8. Reduce/Remove the need for custom applications. Any device with a browser can connect directly to the sensors!
 * 9. Handle (relatively) large volumes of data and file transfers automatically.
 * 
 * *************************************************************************
 * Example Network:
 * 
 * In the following example, 8 Arduino devices have assembled themselves into a
 * wireless mesh network, with 3 sensors attached directly to RPi/Linux. Five 
 * additional sensors are too far away to connect directly to the RPi/Gateway,
 * so they attach automatically to the closest sensor, which will automatically 
 * relay all communications for the distant node.
 * 
 * Example network:
 * 
 * Arduino 4 <-> Arduino 1 <-> Raspberry Pi    <-> Webserver
 * Arduino 5 <->              OR Arduino+Linux <-> Database
 * Arduino 6 <->                               <-> PHP
 *                                             <-> BASH (Wget, Curl, etc)
 * Arduino 7 <-> Arduino 2 <->                 <-> Web-Browser 
 * Arduino 8 <->                               <-> Python
 *               Arduino 3 <->                 <-> NodeJS
 *                                             <-> SSH Tunnel <-> Remote RF24Ethernet Sensor Network
 *                                             <-> VPN        <->
 *               
 * In addition to communicating with external systems, the nodes are able to               
 * communicate internally using TCP/IP, and/or at the RF24Mesh/RF24Network 
 * layers.
 * 
 * **************************************************************************
 * 
 * Example: 
 * 
 * RF24Ethernet Simple Server(Mesh) Example, using RF24Mesh for address allocation
 *
 * This example demonstrates how to send out an HTTP response to a browser.
 *
 *      Documentation: http://tmrh20.github.io/RF24Ethernet/
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
RF24Mesh mesh(radio,network);
RF24EthernetClass RF24Ethernet(radio,network,mesh);


// Set up the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  //printf_begin();
  Serial.println(F("start"));
  
  // Set the IP address we'll be using. The last octet of the IP must be equal
  // to the designated mesh nodeID
  IPAddress myIP(10,10,2,4);
  Ethernet.begin(myIP);
  mesh.begin();
  
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
        Serial.println("*** RENEW ***");
        //refresh the network address        
        mesh.renewAddress();
        
     }else{

        Serial.println("*** MESH OK ***");
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
