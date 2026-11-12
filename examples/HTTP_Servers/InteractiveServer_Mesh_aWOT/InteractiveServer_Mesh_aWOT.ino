/*
 * RF24Ethernet InteractiveServer_Mesh example, adapted to use the aWOT web server library
 * instead of parsing the HTTP request by hand.
 *
 * aWOT (https://awot.net, "aWOT" in the Arduino Library Manager) works with any Arduino
 * Client, so it can sit directly on top of RF24Ethernet. It parses the request line and
 * headers, routes the URL to a handler function, and writes the status line and response
 * headers for us. This sketch was written against aWOT 4.0.0.
 *
 * Requires a version of RF24Ethernet where client.read() keeps the IP stack running when
 * no data is buffered. aWOT waits for the request with read() alone, so with an older
 * RF24Ethernet on the uIP stack (AVR etc.) requests time out.
 *
 * Pages:  /     main page, shows the LED state
 *         /ON   turn the LED on
 *         /OF   turn the LED off
 *         /ST   stats
 *         /CR   credits
 * Anything else (e.g. /favicon.ico) gets a 404 from aWOT.
 */

//*********************** USER CONFIG ***********************
// Comment this out to use the nrf_to_nrf driver for nRF52 radios
#define USE_NRF24
//**********************************************************

#ifdef USE_NRF24
#include <RF24.h>
#include <RF24Network.h>
#include "RF24Mesh.h"
#include <RF24Ethernet.h>
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);
#else
#include <nrf_to_nrf.h>
#include <RF24Network.h>
#include "RF24Mesh.h"
#include <RF24Ethernet.h>
nrf_to_nrf radio;
RF52Network network(radio);
RF52Mesh mesh(radio, network);
RF52EthernetClass RF24Ethernet(radio, network, mesh);
#endif

#include <aWOT.h>

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#if defined(LED_BUILTIN)
#define LED_PIN LED_BUILTIN
#else
#define LED_PIN 2
#endif
#else
#define LED_PIN A3  //Analog pin A3
#endif

// Configure the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

// The aWOT application: holds the routes and processes requests
App app;

bool led_state = 0;

/**********************************************************/

// The HTML is kept in program memory. P() is aWOT's macro for that, used with res.printP().
// Note there is no "HTTP/1.1 200 OK" etc. in here any more: aWOT sends the status line and headers.

P(page_head) = "<!DOCTYPE HTML>\n<html><head><style>\n"
               "body{background-color:linen; text-align: center}"
               "table.center{margin-left:auto;margin-right:auto;}"
               "td{border: 1px solid black;}"
               "</style></head><body>";

P(main_html_p1) = "<b>Hello From Arduino!</b><br>\n"
                  "<br><br> LED/Digital Pin Control:"
                  "<br><br>\n<table class='center'>";

P(main_html_p2) = "<tr><td><a href='/ON'>Turn LED On</a></td>"
                  "<td><a href='/OF'>Turn LED Off</a></td></tr></table>"
                  "<br><a href='/ST'>Stats</a> <a href='/CR'>Credits</a>"
                  "</body></html>";

P(credits_html) = "<b>Credits:</b><br><table class='center'>"
                  "<tr><td>RF24Ethernet by</td><td><a href='https://github.com/tmrh20'>TMRh20</a></td></tr>"
                  "<tr><td>Web server</td><td><a href='https://awot.net'>aWOT</a> by Lasse Lukkari</td></tr>"
                  "<tr><td>Documentation:</td><td><a href='http://nRF24.github.io/RF24Ethernet/'>github.io</a></td></tr>"
                  "</table><br><a href='/'>Home</a></body></html>";

/**********************************************************/

// Route handlers. Each one gets the parsed request and a response to print into.
// The response is an Arduino Print, so print(), println(), F() etc. all work on it.

void main_page(Request& req, Response& res) {
  res.set("Content-Type", "text/html");
  res.printP(page_head);
  res.printP(main_html_p1);

  // Include some variables, print them into the page manually
  if (led_state) {
    res.print(F("<tr><td> </td><td bgcolor=darkseagreen>LED is ON</td></tr>\n"));
  } else {
    res.print(F("<tr><td bgcolor=lightpink>LED is OFF</td><td> </td></tr>\n"));
  }

  res.printP(main_html_p2);
}

void led_on(Request& req, Response& res) {
  led_state = 1;
  digitalWrite(LED_PIN, led_state);
  main_page(req, res);
}

void led_off(Request& req, Response& res) {
  led_state = 0;
  digitalWrite(LED_PIN, led_state);
  main_page(req, res);
}

void stats_page(Request& req, Response& res) {
  uint32_t seconds = millis() / 1000UL;
  uint32_t minutes = seconds / 60UL;
  uint32_t hours = minutes / 60UL;
  uint32_t days = hours / 24UL;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  res.set("Content-Type", "text/html");
  res.printP(page_head);
  res.print(F("<table class='center'><tr><td>Uptime</td><td>"));
  res.print(days);
  res.print(F(" days, "));
  res.print(hours);
  res.print(F(" hours "));
  res.print(minutes);
  res.print(F(" minutes "));
  res.print(seconds);
  res.print(F(" seconds</td></tr><tr><td>IP Buffer Size</td><td>"));
#if USE_LWIP < 1
  res.print(UIP_BUFSIZE);
#else
  res.print(Ethernet.MAX_FRAME_SIZE);
#endif
  res.print(F(" bytes</td></tr><tr><td>aWOT Output<br>Buffer Size</td><td>"));
  res.print(SERVER_OUTPUT_BUFFER_SIZE);
  res.print(F(" bytes</td></tr></table><br><br><a href='/'>Home</a></body></html>"));
}

void credits_page(Request& req, Response& res) {
  res.set("Content-Type", "text/html");
  res.printP(page_head);
  res.printP(credits_html);
}

/**********************************************************/

void setup() {

  Serial.begin(115200);
  Serial.println("start");
  pinMode(LED_PIN, OUTPUT);

  IPAddress myIP(10, 10, 2, 4);
  Ethernet.begin(myIP);
  //Set IP of the RPi (gateway)
  IPAddress gwIP(10, 10, 2, 2);
  Ethernet.set_gateway(gwIP);

  // Un-comment the following line to run "headless" (no RF24Gateway)
  // mesh.setNodeID(0);
  mesh.begin();

  // The routes replace the strncmp() chain of the original example
  app.get("/", &main_page);
  app.get("/ON", &led_on);
  app.get("/OF", &led_off);
  app.get("/ST", &stats_page);
  app.get("/CR", &credits_page);

  // How long aWOT waits for the next byte of a request before giving up on it.
  // The default is 1000ms; a lost packet on a multi-hop mesh can take longer to be re-sent.
  app.setTimeout(3000);

  server.begin();
}

/********************************************************/

uint32_t mesh_timer = 0;

void loop() {

  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if (millis() - mesh_timer > 30000) {  //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if (!mesh.checkConnection()) {
      //refresh the network address
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin();
      }
    }
  }

  // server.available() keeps the IP stack and mesh running, and returns a client once
  // one has sent something
  if (EthernetClient client = server.available()) {
    // Reads the request, calls the matching handler above and sends the response
    app.process(&client);

    client.stop();
    Serial.println(F("********"));
  }

  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}
