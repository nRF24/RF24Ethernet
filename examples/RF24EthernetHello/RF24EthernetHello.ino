/*
 * RF24Ethernet Hello World example, lib initially based on SerialIP
 *
 * SerialIP is a TCP/IP stack that can be used over a serial port (a bit
 * like a dial-up Internet connection, but without the modem.)  It works with
 * stock Arduinos (no shields required.)  When attached to a PC supporting
 * SLIP, the Arduino can host network servers and access the Internet (if the
 * PC is configured to share its Internet connection of course!)
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * For more information see the SerialIP page on the Arduino wiki:
 *   <http://www.arduino.cc/playground/Code/SerialIP>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.5.2 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test.
 *
 * This version also can be easily configured to send out an HTTP response to a browser
 *
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 */
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <printf.h>
#include <RF24Ethernet.h>

/**** NOTE: Radio CE,CS pins must be configured in RF24Ethernet.cpp */
// This code is experimental, so not user friendly
//This sketch will communicate with an RF24Network master node (00)

// The connection_data struct needs to be defined in an external file.
#include "HelloWorldData.h"

void setup() {

  // Set up the speed of our serial link.
  Serial.begin(115200);
  printf_begin();
  //Serial.println("sf");
  //printf_begin();
  // Tell RF24Ethernet which serial port to use (some boards have more than one.)
  // Currently this is limited to HardwareSerial ports, until both it and 
  // SoftwareSerial inherit from a common base class.
  RF24Ethernet.use_device();

  // We're going to be handling uIP events ourselves.  Perhaps one day a simpler
  // interface will be implemented for the Arduino IDE, but until then...  
  RF24Ethernet.set_uip_callback(uip_callback);

  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IP_ADDR myIP = {192,168,1,3};
  IP_ADDR subnet = {255,255,255,0};
  RF24Ethernet.begin(myIP, subnet);

  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  //IP_ADDR gwIP = {192,168,5,1};
  //RF24Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.  Each incoming
  // connection will result in the uip_callback() function being called.
  RF24Ethernet.listen(1000);
}

void loop() {
  // Check the serial port and process any incoming data.
  RF24Ethernet.tick();

  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

void uipudp_appcall(){
}

void uip_callback(uip_tcp_appstate_t *s)
{
  if (uip_connected()) {

    // We want to store some data in our connections, so allocate some space
    // for it.  The connection_data struct is defined in a separate .h file,
    // due to the way the Arduino IDE works.  (typedefs come after function
    // definitions.)
    connection_data *d = (connection_data *)malloc(sizeof(connection_data));

    // Save it as RF24Ethernet user data so we can get to it later.
    s->user = d;

    // The protosocket's read functions need a per-connection buffer to store
    // data they read.  We've got some space for this in our connection_data
    // structure, so we'll tell uIP to use that.
    PSOCK_INIT(&s->p, d->input_buffer, sizeof(d->input_buffer));

  }

  // Call/resume our protosocket handler.
  handle_connection(s, (connection_data *)s->user);

  // If the connection has been closed, release the data we allocated earlier.
  if (uip_closed()) {
    if (s->user) free(s->user);
    s->user = NULL;
  }
}

// This function is going to use uIP's protosockets to handle the connection.
// This means it must return int, because of the way the protosockets work.
// In a nutshell, when a PSOCK_* macro needs to wait for something, it will
// return from handle_connection so that other work can take place.  When the
// event is triggered, uip_callback() will call this function again and the
// switch() statement (see below) will take care of resuming execution where
// it left off.  It *looks* like this function runs from start to finish, but
// that's just an illusion to make it easier to code :-)
//int handle_connection(uip_tcp_appstate_t *s, connection_data *d)
int handle_connection(uip_tcp_appstate_t *s, connection_data *d)
{
  // All protosockets must start with this macro.  Its internal implementation
  // is that of a switch() statement, so all code between PSOCK_BEGIN and
  // PSOCK_END is actually inside a switch block.  (This means for example,
  // that you can't declare variables in the middle of it!)
  PSOCK_BEGIN(&s->p);

  PSOCK_SEND_STR(&s->p, "HTTP/1.1 200 OK\n");
  PSOCK_SEND_STR(&s->p, "Content-Type: text/html\n");
  PSOCK_SEND_STR(&s->p, "Connection: close\n");
  PSOCK_SEND_STR(&s->p, "Refresh: 5\n");
  PSOCK_SEND_STR(&s->p, "\n");
  PSOCK_SEND_STR(&s->p, "<!DOCTYPE HTML>\n");
  PSOCK_SEND_STR(&s->p, "<html>\n");
  PSOCK_SEND_STR(&s->p, "HELLO FROM ARDUINO!\n");
  PSOCK_SEND_STR(&s->p, "</html>\n");
  
  // Read some returned text into the input buffer we set in PSOCK_INIT.  Data
  // is read until a newline (\n) is received, or the input buffer gets filled
  // up.  (Which, at 16 chars by default, isn't hard!)

  // Send some text over the connection.
  //PSOCK_SEND_STR(&s->p, "Hello. What is your name?\n");
  //PSOCK_READTO(&s->p, '\n');

  // Since any subsequent PSOCK_* functions would overwrite the buffer, we
  // need to make a copy of it first.  We can't use a local variable for this,
  // because any PSOCK_* macro may make the function return and resume later,
  // thus losing the data in any local variables.  This is why uip_callback
  // has allocated the connection_data structure for us to use instead.  (You
  // can add/remove other variables in this struct to store different data.
  // See the other file in this sketch, serialip_conn.h)
  //strncpy(d->name, d->input_buffer, sizeof(d->name));
  // Note that this will misbehave when the input buffer overflows (i.e.
  // more than 16 characters are typed in) but hey, this is supposed to be
  // a simple example.  Fixing this problem will be left as an exercise for
  // the reader :-)

  // Send some more data over the connection.
  //PSOCK_SEND_STR(&s->p, "Hello ");

  memcpy(d->name,d->input_buffer,PSOCK_DATALEN(&s->p));
  PSOCK_SEND_STR(&s->p, d->name);
  //PSOCK_SEND_STR(&s->p, "wtf ");
  // Disconnect.
  PSOCK_CLOSE(&s->p);

  // All protosockets must end with this macro.  It closes the switch().
  PSOCK_END(&s->p);
}

