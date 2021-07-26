/*
 *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2015

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://nRF24.github.io/RF24Ethernet/

 *************************************************************************
 *
 **** EXAMPLE REQUIRES: PubSub MQTT library: https://github.com/knolleary/pubsubclient ***
 *
 * Install using the Arduino library manager
 *
 *************************************************************************
  Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - Continually publishes its nodeID to the outTopic

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <PubSubClient.h>

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

IPAddress ip(10, 10, 2, 4);
IPAddress gateway(10, 10, 2, 2); //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2); //The ip of the MQTT server

char clientID[] = {"arduinoClient   "};

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      uint32_t recTimer = millis();
      while (millis() - recTimer < 5000) {
        Ethernet.update();
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(ip);
  Ethernet.set_gateway(gateway);
  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  //Convert the last octet of the IP address to an identifier used
  char str[4];
  int test = ip[3];
  itoa(ip[3], str, 10);
  memcpy(&clientID[13], &str, strlen(str));
  Serial.println(clientID);

}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;

void loop()
{
  Ethernet.update();

  if (millis() - mesh_timer > 30000) { //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if ( ! mesh.checkConnection() ) {
      if (!mesh.renewAddress()) {
        mesh.begin(MESH_DEFAULT_CHANNEL, RF24_1MBPS, 5000);
      }
    }
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Every second, report to the MQTT server the Node ID of this node
  if (client.connected() && millis() - pub_timer > 1000) {
    pub_timer = millis();
    char str[4];
    int test = ip[3];
    itoa(ip[3], str, 10);
    char str1[] = "Node      \r\n";
    memcpy(&str1[5], &str, strlen(str));

    client.publish("outTopic", str1);
  }


}