/*
 * *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2015

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://tmrh20.github.io/RF24Ethernet/

 * *************************************************************************

   RF24Ethernet web client example

   In order to minimize memory use and program space:
   1. Open the RF24Network library folder
   2. Edit the RF24Networl_config.h file
   3. Un-comment #define DISABLE_USER_PAYLOADS

   This example connects to google and downloads the index page
*/


#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>

/*** Configure the radio CE & CS pins ***/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);


EthernetClient client;

void setup() {

  Serial.begin(115200);
  Serial.print("Start Mesh:");

  // Set the IP address we'll be using. The last octet mast match the nodeID (9)
  IPAddress myIP(10, 10, 2, 4);
  Ethernet.begin(myIP);

  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10, 10, 2, 2);
  Ethernet.set_gateway(gwIP);
}

uint32_t counter = 0;
uint32_t reqTimer = 0;

uint32_t mesh_timer = 0;

bool gotHeader = 0;
long contentLength = 0;

void loop() {

  if (millis() - mesh_timer > 30000) { //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if ( ! mesh.checkConnection() ) {
      mesh.renewAddress();
    }
  }

  size_t size;

  if ( (size = client.available()) > 0) {

    //Look for http header with content length. This will empty the entire request
    //if the server does not include the length.
    if (!gotHeader) {
      if (client.find("Content-Length: ")) {
        contentLength = client.parseInt();
        client.find("\r\n\r\n");
        counter = 0;
        gotHeader = true;
      }
      //If the header was already received, read the data & verify the length
    } else {
      if (client.available() > 0) {
        char c = client.read();
        Serial.print(c);
        counter++;
      }
      if (counter == contentLength) {
        gotHeader = false;
        Serial.print("Content length "); Serial.println(contentLength);
        Serial.print("Received length "); Serial.println(counter);
      }
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("Disconnect. Waiting for disconnect timeout"));
    client.stop();

    // Wait 5 seconds between requests
    reqTimer = millis();
    while (millis() - reqTimer < 5000 && !client.available() ) { }
    connect();

  }
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

void connect() {
  Serial.println(F("connecting"));
  IPAddress pizza(94, 199, 58, 243);
  if (client.connect(pizza, 80)) {
    Serial.println(F("connected"));

    // Make an HTTP request:
    client.write("GET /asciiart/pizza.txt HTTP/1.1\n");
    client.write("Host: fiikus.net\n");
    client.write("Connection: close\n\n");

  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("connection failed"));
  }
}

