/*
 * RF24Ethernet InteractiveServer example by TMRh20
 * - Webserver controlling an LED example
 *
 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a Headless Master node running an HTTP server
 * It does NOT require a Gateway node, just two nodes running RF24Ethernet
 *
 */

#define OUTPUT_BUFFER_SIZE MAX_PAYLOAD_SIZE - 14



//*********************** USER CONFIG ***********************
// Comment this out to use the nrf_to_nrf driver for nRF52 radios
#define USE_NRF24
//**********************************************************

#ifdef USE_NRF24
#include <RF24.h>
#include <RF24Network.h>
#include "RF24Mesh.h"
#include <RF24Ethernet.h>
#include "HTML.h"

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);
#else
#include <nrf_to_nrf.h>
#include <RF24Network.h>
#include "RF24Mesh.h"
#include <RF24Ethernet.h>
#include "HTML.h"
nrf_to_nrf radio;
RF52Network network(radio);
RF52Mesh mesh(radio, network);
RF52EthernetClass RF24Ethernet(radio, network, mesh);
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#define LED_PIN BUILTIN_LED
#else
#define LED_PIN A3  //Analog pin A3
#endif

// Configure the server to listen on port 1000
EthernetServer server = EthernetServer(80);

/**********************************************************/
static unsigned short generate_tcp_stats();

void setup() {

  Serial.begin(115200);
  while (!Serial) {}
  //printf_begin();
  Serial.println("start");
  pinMode(LED_PIN, OUTPUT);

  IPAddress myIP(10, 1, 3, 1);
  Ethernet.begin(myIP);

  //Set IP of the RPi (gateway)
  IPAddress gwIP(10, 1, 3, 2);
  Ethernet.set_gateway(gwIP);

  // Make this a Master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  mesh.begin(65);

  server.begin();
  server.setTimeout(30000);
}

/********************************************************/

uint32_t mesh_timer = 0;

void loop() {

  Ethernet.update();

  if (EthernetClient client = server.available()) {
    uint8_t pageReq = 0;
    char requestBuffer[20];
    memset(requestBuffer, 0, sizeof(requestBuffer));

    uint16_t bytesRead = 0;
    uint32_t timeout = millis();

    while (client.connected() && (millis() - timeout < 100)) {
      if (client.available()) {
        char c = client.read();
        if (c == '/') {
          if (client.available() >= 2) {
            client.readBytes(requestBuffer, 2);
          }
          break;
        }
      }
    }

    if (strncmp(requestBuffer, "ON", 2) == 0) {
      led_state = 1;
      pageReq = 1;
    } else if (strncmp(requestBuffer, "OF", 2) == 0) {
      led_state = 0;
      pageReq = 1;
    } else if (strncmp(requestBuffer, "ST", 2) == 0) {
      pageReq = 2;
    } else if (strncmp(requestBuffer, "CR", 2) == 0) {
      pageReq = 3;
    } else {
      pageReq = 4;  // Default to main
    }

    client.flush();

    switch (pageReq) {
      case 2: stats_page(client); break;
      case 3: credits_page(client); break;
      default: main_page(client); break;
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
static unsigned short generate_tcp_stats() {
  /*struct uip_conn* conn;

  // If multiple connections are enabled, get info for each active connection
  for (uint8_t i = 0; i < UIP_CONF_MAX_CONNECTIONS; i++) {
    conn = &uip_conns[i];

    // If the application state is active for an available connection, print the info
    if (conn->appstate) {
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
  }*/
  return 1;
}
