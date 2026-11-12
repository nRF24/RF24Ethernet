/*
 * *************************************************************************
 * RF24Ethernet Arduino library by TMRh20 - 2014-2015
 *
 * Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules
 *
 * RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
 *                     -> RF24Mesh
 *
 *      Documentation: http://nRF24.github.io/RF24Ethernet/
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
 * RF24Ethernet simple web client example
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * In order to minimize memory use and program space:
 * 1. Open the RF24Network library folder
 * 2. Edit the RF24Network_config.h file
 * 3. Un-comment #define DISABLE_USER_PAYLOADS
 * 4. Remember to set it back for normal operation
 *
 * This example will get you some pizza and a book to read
 *
 */

#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
//#include <printf.h>

/*** Configure the radio CE & CS pins ***/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

EthernetClient client;

// The hosts we will be connecting to
// Note: The gateway will need to be able to forward traffic for internet hosts, see the documentation
IPAddress icewind(109, 120, 203, 163);  //http://109.120.203.163/web/blyad.club/library/litrature/Salvatore,%20R.A/Salvatore,%20R.A%20-%20Icewind%20Dale%20Trilogy%201%20-%20Crystal%20Shard,%20The.txt
IPAddress ascii(208, 86, 224, 90);      //http://artscene.textfiles.com/asciiart/texthistory.txt
IPAddress host(ascii);

void setup() {

  Serial.begin(115200);
  //printf_begin();
  Serial.println(F("Start"));

  // Set the IP address we'll be using. The last octet mast match the nodeID (9)
  IPAddress myIP(10, 10, 2, 4);
  Ethernet.begin(myIP);
  mesh.begin();

  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10, 10, 2, 2);
  Ethernet.set_gateway(gwIP);
}

uint32_t counter = 0;
uint32_t reqTimer = 0;
uint32_t mesh_timer = 0;

void loop() {

  // Send a p or g character over serial to switch between hosts
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'p') {
      host = ascii;
    } else if (c == 'g') {
      host = icewind;
    }
  }

  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if (millis() - mesh_timer > 12000) {  //Every 12 seconds, test mesh connectivity
    mesh_timer = millis();
    if (!mesh.checkConnection()) {
      //refresh the network address
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin();
      }
    }
  }

  size_t size;

  if ((size = client.available()) > 0) {
    char c = client.read();
    Serial.print(c);
    counter++;
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("Disconnect. Waiting for disconnect timeout"));
    client.stop();

    // Wait 5 seconds between requests
    // Calling client.available(); or Ethernet.update(); is required during delays
    // to keep the stack updated
    reqTimer = millis();
    while (millis() - reqTimer < 5000 && !client.available()) {
    }
    connect();
  }

  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

void connect() {
  Serial.println(F("connecting"));

  if (client.connect(host, 80)) {
    Serial.println(F("connected"));

    // Make an HTTP request:
    if (host == ascii) {
      client.println("GET /asciiart/texthistory.txt HTTP/1.1");
      client.println("Host: artscene.textfiles.com");
    } else {
      client.println("GET /web/blyad.club/library/litrature/Salvatore,%20R.A/Salvatore,%20R.A%20-%20Icewind%20Dale%20Trilogy%201%20-%20Crystal%20Shard,%20The.txt HTTP/1.1");
      client.println("Host: 109.120.203.163");
    }

    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("connection failed"));
  }
}
