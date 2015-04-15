/*
 * TMRh20 2014
 * 
 * RF24Ethernet simple web client example
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * In order to minimize memory use and program space:
 * 1. Open the RF24Network library folder
 * 2. Edit the RF24Networl_config.h file
 * 3. Un-comment #define DISABLE_USER_PAYLOADS
 *
 * This example connects to google and downloads the index page
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
//#include <printf.h>
#include <RF24Ethernet.h>

/*** Configure the radio CE & CS pins ***/
RF24 radio(7,8);
RF24Network network(radio);
RF24EthernetClass RF24Ethernet(radio,network);


EthernetClient client;

void setup() {
  
  Serial.begin(115200);
 // printf_begin();
  
  // Tell RF24Ethernet which channel to use. This step is not that important, but it 
  // is important to set the channel this way, not directly via the radio
  //Ethernet.setChannel(97);
  
  // This step is very important. The address of the node needs to be set both
  // on the radio and in the UIP layer
  // This is the RF24Network address and needs to be configured accordingly if
  // using more than 4 nodes with the master node. Otherwise, 01-04 can be used.
  uint16_t myRF24NetworkAddress = 01;
  Ethernet.setMac(myRF24NetworkAddress);
  
  //Optional
  //radio.setPALevel(RF24_PA_HIGH);
  //radio.printDetails(); // Requires printf
  
  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IPAddress myIP(10,10,2,4);
  Ethernet.begin(myIP);
  
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10,10,2,2);
  Ethernet.set_gateway(gwIP);  
}

uint32_t counter = 0;
uint32_t reqTimer = 0;

void loop() {

size_t size;

if(size = client.available() > 0){
    char c = client.read();
    Serial.print(c);
    // Sends a line-break every 150 characters, comment out if not connecting to google
    if(counter > 150){ Serial.println(""); counter=0;}
    counter++;
}

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("Disconnect. Waiting for disconnect timeout"));
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
    IPAddress goog(74,125,224,87);
    IPAddress pizza(94,199,58,243);
    if (client.connect(goog, 80)) {
      Serial.println(F("connected"));
      
      // Make an HTTP request:
      //client.write("GET /asciiart/pizza.txt HTTP/1.1\n");
      client.write("GET / HTTP/1.1\n");
      
      //client.write("Host: fiikus.net\n");
      client.write("Host: www.google.ca\n");
      
      client.write("Connection: close\n");
      client.println();    
    
    }else{
      // if you didn't get a connection to the server:
      Serial.println(F("connection failed"));
    }
}
