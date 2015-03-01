/*
 * RF24Ethernet Getting_Started_InteractiveServer example by TMRh20
 * - Webserver controlling an LED example
 *
 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a sensor node to act as a webserver and
 * allows a user to control a connected LED by clicking links on the webpage
 * The requested URL is used as input, to determine whether to turn the LED off or on
 *
 * In order to minimize memory use and program space:
 * 1. Open the RF24Network library folder
 * 2. Edit the RF24Networl_config.h file
 * 3. Un-comment #define DISABLE_USER_PAYLOADS
 * 4. See the Getting_Started_SimpleServer_Minimal example
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

#define LED_PIN A3 //Analog pin A3


// Configure the server to listen on port 1000
EthernetServer server = EthernetServer(1000);

void setup() {
  // Set up the speed of our serial link.
  Serial.begin(115200);
  //printf_begin();
  Serial.println(F("start"));
  pinMode(LED_PIN,OUTPUT); 
  
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
  //radio.printDetails();
  
  // Set the IP address we'll be using.  Make sure this doesn't conflict with
  // any IP addresses or subnets on your LAN or you won't be able to connect to
  // either the Arduino or your LAN...
  IPAddress myIP(10,10,2,4);
  Ethernet.begin(myIP);
  
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10,10,2,2);
  Ethernet.set_gateway(gwIP);

  // Listen for incoming connections on TCP port 1000.  Each incoming
  // connection will result in the uip_callback() function being called.
  server.begin();
}


bool led_state = 0;
uint32_t timer = 0;

void loop() {

  size_t size;

  if(EthernetClient client = server.available())  
  {
      while((size = client.available()) > 0)
      {       
        // If a request is received with enough characters, search for the / character
        if(size >= 7){
            client.findUntil("/","/");
            char buf[3] = {"  "};
            buf[0] = client.read();  // Read in the first two characters from the request
            buf[1] = client.read();
            
            if(strcmp(buf,"ON")==0){ // If the user requested http://ip-of-node:1000/ON
              Serial.println(F("TURN ON"));
              led_state = 1;
              digitalWrite(LED_PIN,led_state);
            }else
            if(strcmp(buf,"OF")==0){ // If the user requested http://ip-of-node:1000/OF
               Serial.println(F("TURN OFF"));
               led_state = 0;
               digitalWrite(LED_PIN,led_state);
            }
        }
        // Empty the rest of the data from the client
        while(client.waitAvailable()){
           client.read(); 
        }
       }

        char stateLine[24];
        int len = sprintf(stateLine, "The LED state is %d\n<br>",led_state);	
        
        client.write( "HTTP/1.1 200 OK\nContent-Type: text/html\n Connection: close\n\n<!DOCTYPE HTML>\n<html>\n");
        client.write( "Hello From Arduino!<br>\n");
        client.write( stateLine );
        client.write( "<a href='/ON'>Turn LED On</a><br>\n <a href='/OF'>Turn LED Off</a><br>\n</html>\n");

       client.stop(); 
       Serial.println(F("********"));
          
    }
 
  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}


