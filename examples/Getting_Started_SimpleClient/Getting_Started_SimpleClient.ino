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
//#include <printf.h>
 #include <RF24Ethernet.h>


/**** NOTE: Radio CE,CS pins must be configured in RF24Ethernet.cpp */
// This code is experimental, so not user friendly
//This sketch will communicate with an RF24Network master node (00)

// The connection_data struct needs to be defined in an external file.
//#include "HelloWorldData.h"

RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio,network);


//EthernetServer server = EthernetServer(1000);
//EthernetServer server2 = EthernetServer(80);

EthernetClient client;

void setup() {
  
  Serial.begin(115200);
  //printf_begin();
  // This initializes the radio with basic settings.
  // Needs to be called at the beginning of every sketch
  Ethernet.use_device();  
  
  // Tell RF24Ethernet which channel to use. This step is not that important, but it 
  // is important to set the channel this way, not directly via the radio
  //Ethernet.setChannel(97);
  
  // This step is very important. The address of the node needs to be set both
  // on the radio and in the UIP layer
  // This is the RF24Network address and needs to be configured accordingly if
  // using more than 4 nodes with the master node. Otherwise, 01-04 can be used.
  uint16_t myRF24NetworkAddress = 04;
  Ethernet.setMac(myRF24NetworkAddress);
  
  //Optional
  radio.setPALevel(RF24_PA_HIGH);
  //radio.printDetails();
  
  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IP_ADDR myIP = {10,10,2,3};
  IP_ADDR subnet = {255,255,255,0};
  Ethernet.begin(myIP, subnet);
  
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IP_ADDR gwIP = {10,10,2,2};
  Ethernet.set_gateway(gwIP);  
}

uint32_t testTimer = 0;
uint32_t counter = 0;
uint32_t reqTimer = 0;
uint32_t restartTimer = 0;
bool first = 1;
void loop() {

//  if(millis() - testTimer > 5000){
//     testTimer = millis();
//     Serial.println(F("alive")); 
//  }
  uint8_t c=0;
  size_t size;

if(size = client.available() > 0){
    //char c = client.read();
    //Serial.print(c);
    uint8_t buf[size];
    client.read(buf,size);
    Serial.write(buf,size);
    if(counter > 100){ /*Serial.println("");*/ counter=0;}
    counter++;
    //delay(4);
    restartTimer = millis();
}else{
   if(millis()-restartTimer > 1000){
      restartTimer = millis();
      //uip_restart();
      //Serial.println("restart manual");
   } 
}

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("Disconnect. Waiting for serial input"));
    client.stop();
  
    // Wait for serial input to begin
    //while (!Serial.available() && !client.connected()){}
    //Serial.read();
    reqTimer = millis();
    while(millis() - reqTimer < 5000 && !client.available() ){ }    
    connect();
  
  }
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

void connect(){
    Serial.println(F("connecting"));
    IPAddress goog(74,125,224,72);
    IPAddress purp(83,227,225,7);
    IPAddress loc(10,10,1,49);
    IPAddress art(80,237,132,189);
    IPAddress pizza(94,199,58,243);
    //IPAddress reddit(198,41,209,137);
    if (client.connect(purp, 80)) {
      Serial.println(F("connected"));
      // Make a HTTP request:
      //client.println(F("GET /search?q=arduino HTTP/1.1"));
      //delay(100);
      //client.println(F("GET / HTTP/1.1"));
      //client.println(F("Host: www.google.ca"));
      //client.println(F("Connection: close"));
      
      //uint8_t buffr[39] = {"GET / HTTP/1.1\n"};
      //uint8_t buffr[39] = {"GET /asciiart/pizza.txt HTTP/1.1\n"}; //Length is 15 bytes
      //uint8_t buffr[32] = {"GET /ascii/c/cat.txt HTTP/1.1\n"}; //Length is 30 bytes
      uint8_t buffr[33] = {"GET /taiji/ascii.txt HTTP/1.1\n"};
      
      client.write(buffr,30);
      //memcpy(buffr,"Host: www.google.ca\n",20);
      memcpy(buffr,"Host: hakank.org\n",17);
      //memcpy(buffr,"Host: fiikus.net\n",17);
      //client.println(F("GET /r/funny/.rss HTTP/1.1"));
      client.write(buffr,17);
      memcpy(buffr,"Connection: close\n",18);
      //buffr = "Connection: close\n";
      client.write(buffr,18);
      client.println();    
      //delay(100);
    
    }else{
      // if you didn't get a connection to the server:
      Serial.println(F("connection failed"));
    }
}


