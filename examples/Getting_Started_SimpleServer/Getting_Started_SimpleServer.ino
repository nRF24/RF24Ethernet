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
//#include "HelloWorldData.h"

RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio,network);


EthernetServer server = EthernetServer(1000);
//EthernetServer server2 = EthernetServer(80);
void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  printf_begin();
  Serial.println("start");

  // This initializes the radio with basic settings.
  // Needs to be called at the beginning of every sketch
  Ethernet.use_device();  
  
  // Tell RF24Ethernet which channel to use. This step is not that important, but it 
  // is important to set the channel this way, not directly via the radio
  Ethernet.setChannel(97);
  
  // This step is very important. The address of the node needs to be set both
  // on the radio and in the UIP layer
  // This is the RF24Network address and needs to be configured accordingly if
  // using more than 4 nodes with the master node. Otherwise, 01-04 can be used.
  uint16_t myRF24NetworkAddress = 02;
  Ethernet.setMac(myRF24NetworkAddress);
  
  //Optional
  radio.setPALevel(RF24_PA_LOW);
  radio.printDetails();
  
  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IPAddress myIP(10,10,2,3);
  Ethernet.begin(myIP);
  
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10,10,2,2);
  Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.  Each incoming
  // connection will result in the uip_callback() function being called.
  server.begin();
}

uint16_t testTimer = 0;

void loop() {

  uint8_t c=0;
  
  size_t size;

  if(EthernetClient client = server.available())  
  {

      while((size = client.available()) > 0)
      {                 
        uint8_t msg[45];
        //size = client.read(msg,size);
        char c;
        //while(c = client.read() > 0){
          Serial.print((char)client.read());
        //}
        Serial.println("");
        //Serial.write(msg,size);
       }
	client.write( "HTTP/1.1 200 OK\n");
        client.write( "Content-Type: text/html\n");
        client.write( "Connection: close\n");
        client.write( "Refresh: 5\n");
        client.write( "\n");
        client.write( "<!DOCTYPE HTML>\n");
        client.write( "<html>\n");
        client.write( "HELLO FROM ARDUINO!\n");
        client.write( "</html>\n");
	   
       testTimer = millis();
       client.stop(); 
       Serial.println("********");
       delay(100);
           
    }
 
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

