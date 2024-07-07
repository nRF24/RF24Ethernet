/*
 *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2021

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://nRF24.github.io/RF24Ethernet/

 *************************************************************************

 **** EXAMPLE REQUIRES: Arduino MQTT library: https://github.com/256dpi/arduino-mqtt/ ***
   Shown in Arduino Library Manager as 'MQTT' by Joel Gaehwiler

 *************************************************************************
  Basic MQTT example

  This sketch demonstrates the basic capabilities of the library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives.
  - it assumes the received payloads are strings not binary
  - Continually publishes its nodeID to the outTopic

  It will reconnect to the server if the connection is lost using a non-blocking
  reconnect function.

*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <MQTT.h>

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

IPAddress ip(10, 10, 2, 4);
IPAddress gateway(10, 10, 2, 2);  //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);   //The ip of the MQTT server
char clientID[] = { "arduinoClient   " };

void messageReceived(MQTTClient* client, char topic[], char payload[], int length) {
  (void)*client;
  (void)length;
  Serial.println("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(payload);
}

EthernetClient ethClient;
MQTTClient client;

void connect() {
  Serial.println("connecting...");
  if (!client.connect(clientID)) {
    mesh.renewAddress();
    return;
  }
  Serial.println("\nconnected!");
  client.publish("outTopic", "hello world");
  client.subscribe("inTopic", 2);
}

void setup() {

  Serial.begin(115200);

  Ethernet.begin(ip, gateway);

  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  //Convert the last octet of the IP address to an identifier used
  char str[4];
  itoa(ip[3], str, 10);
  memcpy(&clientID[13], &str, strlen(str));
  Serial.println(clientID);

  client.begin(server, ethClient);
  client.onMessageAdvanced(messageReceived);
}


uint32_t pub_timer = 0;

void loop() {

  Ethernet.update();

  if (!client.connected()) {
    if (!mesh.checkConnection()) {
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin();
      }
    }
    connect();
    Serial.println();
  }

  client.loop();

  // Every so often, report to the MQTT server the Node ID of this node
  if (client.connected() && millis() - pub_timer > 3000) {
    pub_timer = millis();
    char str[4];
    itoa(ip[3], str, 10);
    char str1[] = "Node      \r\n";
    memcpy(&str1[5], &str, strlen(str));

    client.publish("outTopic", str1);
  }
}
