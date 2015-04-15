/*
 * TMRh20 2014
 * 
 * RF24Ethernet simple web client using DNS example
 *
 * RF24Ethernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * In order to use DNS, UDP must be enabled in the uIP stack:
 * 1. Open the RF24Ethernet library folder
 * 2. Edit the uip_conf.h file
 * 3. Set #define UIP_CONF_UDP 1
 * 4. The gateway device must be able to forward DNS requests to the DNS server
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
  
  uint16_t myRF24NetworkAddress = 01;
  Ethernet.setMac(myRF24NetworkAddress);
  
  IPAddress myIP(10,10,2,4);
  IPAddress dnsIP(192,168,1,1);
  Ethernet.begin(myIP,dnsIP);
  
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

    if (client.connect("www.google.com", 80)) {
      Serial.println(F("connected"));
      
      // Make an HTTP request:
      client.write("GET / HTTP/1.1\n");      
      client.write("Host: www.google.ca\n");      
      client.write("Connection: close\n");
      client.println();    
    
    }else{
      // if you didn't get a connection to the server:
      Serial.println(F("connection failed"));
    }
}
