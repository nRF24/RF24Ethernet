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
 * This example demonstrates how to configure a Headless Master node running an HTTP server
 * It does NOT require a Gateway node, just two nodes running RF24Ethernet
 *
 */

#include <nrf_to_nrf.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
//#include <printf.h>

/*** Configure the radio CE & CS pins ***/
nrf_to_nrf radio;
RF52Network network(radio);
RF52Mesh mesh(radio, network);
RF52EthernetClass RF24Ethernet(radio, network, mesh);

EthernetClient client;

// The hosts we will be connecting to
// Note: The gateway will need to be able to forward traffic for internet hosts, see the documentation
IPAddress ascii(10, 1, 3, 1);      //http://artscene.textfiles.com/asciiart/texthistory.txt
IPAddress host(ascii);

void setup() {

  Serial.begin(115200);
  //printf_begin();
  while(!Serial){}
  Serial.println(F("Start"));

  // Set the IP address we'll be using. The last octet mast match the nodeID (9)
  IPAddress myIP(10, 1, 3, 2);
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10, 1, 3, 1);
  Ethernet.set_gateway(gwIP);

  Ethernet.begin(myIP);
  mesh.begin(65);


}

uint32_t counter = 0;
uint32_t reqTimer = 0;
uint32_t mesh_timer = 0;

void loop() {


  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if (millis() - mesh_timer > 12000) {  //Every 12 seconds, test mesh connectivity
    mesh_timer = millis();
    if (!mesh.checkConnection()) {
      Serial.println("Renew");
      //refresh the network address
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin(65);
      }
    }
  }

  size_t size;

  while ((size = client.available()) > 0) {
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
      client.println("GET / HTTP/1.1");
      client.println("Host: 10.1.3.1");
    }

    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("connection failed"));
  }
}
