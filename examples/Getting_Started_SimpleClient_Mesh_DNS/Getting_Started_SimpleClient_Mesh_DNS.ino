/*
 * TMRh20 2021
 *
 * RF24Ethernet simple web client using DNS example
 *
 * In order to use DNS, UDP must be enabled in the uIP stack:
 * 1. Open the RF24Ethernet library folder
 * 2. Edit the uip_conf.h file
 * 3. Uncomment #define UIP_CONF_UDP 1
 * 4. The gateway device must be able to forward DNS requests to the DNS server
 *
 * This example connects to get you a pizza or a book to read using DNS lookups instead of IP address
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
// Note: DNS responses for www.domain.com will typically be shorter than requests for domain.com.
char icewind[] = { "109.120.203.163" };  //http://109.120.203.163/web/blyad.club/library/litrature/Salvatore,%20R.A/Salvatore,%20R.A%20-%20Icewind%20Dale%20Trilogy%201%20-%20Crystal%20Shard,%20The.txt
char ascii[] = { "artscene.textfiles.com" };  //http://artscene.textfiles.com/asciiart/texthistory.txt
char* host = ascii;

void setup() {

  Serial.begin(115200);
  //printf_begin();
  Serial.println(F("Start"));

  // Set the IP address we'll be using. The last octet mast match the nodeID (9)
  IPAddress myIP(10, 10, 2, 4);
  IPAddress myDNS(8, 8, 8, 8);  //Use Google DNS in this example
  Ethernet.begin(myIP, myDNS);
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
