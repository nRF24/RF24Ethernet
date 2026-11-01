/*
 *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2026

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://nRF24.github.io/RF24Ethernet/

 *************************************************************************
 *
 **** EXAMPLE REQUIRES: ***
 *  - Arduino MQTT library: https://github.com/256dpi/arduino-mqtt/
 *    Shown in Arduino Library Manager as 'MQTT' by Joel Gaehwiler
 *  - SSLClient (mbedTLS wrapper for any Arduino Client, ESP32):
 *    https://github.com/govorox/SSLClient
 *    Install from ZIP. NOTE: remove the OPEnSLab 'SSLClient' library first,
 *    both use the same name/header.
 *
 *************************************************************************
  Basic MQTT over TLS example for ESP32

  This sketch is the TLS version of the mqtt_basic example. The TCP
  connection provided by RF24Ethernet is wrapped in a TLS layer (mbedTLS)
  before being handed to the MQTT client:

      MQTTClient -> SSLClient (TLS) -> EthernetClient (RF24Ethernet/uIP)

  It connects to an MQTT server on port 8883 then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives.
  - it assumes the received payloads are strings not binary
  - Continually publishes its nodeID to the outTopic

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function.

  Broker side (mosquitto example):
    listener 8883
    cafile   /etc/mosquitto/certs/ca.crt
    certfile /etc/mosquitto/certs/server.crt
    keyfile  /etc/mosquitto/certs/server.key

  WiFi credentials, MQTT credentials and TLS certificates are kept in
  secrets.h. Edit that file with your own values before uploading.

  NOTE: The TLS handshake transfers several KB (certificate chain) and is
  CPU heavy, so over the RF24 link it can take a number of seconds. Timeouts
  below have been raised accordingly.
*/

#include <SPI.h>
#include <WiFi.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <SSLClient.h>
#include <MQTT.h>
#include "printf.h"
#include "secrets.h"
#include "temporaryWrapper.h"

RF24 radio(17, 5);  // CE, CSN
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

/**** MQTT configuration ****/
#define MQTT_TLS_PORT 8883
// Broker username/password are set in secrets.h

/**** TLS configuration ****/
// Uncomment to skip server certificate verification (encryption only, NO authentication).
// Useful for a first test, not recommended otherwise.
//#define TLS_INSECURE

// Uncomment to use client certificate authentication (mosquitto: require_certificate true)
// The certificate and key themselves are set in secrets.h
//#define TLS_CLIENT_CERT

IPAddress ip(10, 10, 2, 4);
IPAddress gateway(10, 10, 2, 2);  //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);   //The ip of the MQTT server
char clientID[20];

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(payload);
}

/**** WiFi ****/
// Blocking connect with a timeout
bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  Serial.print("Connecting to WiFi: ");
  Serial.println(SECRET_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  const uint32_t wifiTimeoutStart = millis();
  uint32_t dotTimer = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - dotTimer > 500) {
      dotTimer = millis();
      Serial.print(".");
    }
    if (millis() - wifiTimeoutStart > 15000) {
      Serial.println("\nWiFi connection failed");
      return false;
    }
  }
  Serial.print("\nWiFi connected, IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

EthernetClient ethClient;
TemporaryClientWrapper tlsTransport(ethClient);
// TLS layer on top of the RF24Ethernet TCP client
SSLClient sslClient(&tlsTransport);
MQTTClient client;

void connect() {
  Serial.print("connecting (TLS)...");
  const uint32_t clTimeoutStart = millis();
#if defined SECRET_MQTT_USER
  while (!client.connect(clientID, SECRET_MQTT_USER, SECRET_MQTT_PASS)) {
#else
  while (!client.connect(clientID)) {
#endif
    Serial.print(".");
    Serial.print(" lastError: ");
    Serial.print(client.lastError());
    Serial.print(" returnCode: ");
    Serial.println(client.returnCode());
    // Make sure a half-open TLS session is torn down before retrying
    sslClient.stop();
    if (millis() - clTimeoutStart > 30001) {
      Serial.println();
      return;
    }
    const uint32_t timerStart = millis();
    //Instead of delay, keep the RF24 stack updating
    while (millis() - timerStart < 1000) {
      Ethernet.update();
    }
  }

  Serial.println("\nconnected!");
  client.publish("outTopic", "hello world");
  client.subscribe("inTopic", 2);
}

void setup() {
  Serial.begin(115200);

  printf_begin();
  connectWiFi();
  Ethernet.begin(ip, gateway);

  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  //Convert the last octet of the IP address to an identifier used
  snprintf(clientID, sizeof(clientID), "arduinoClient%u", (uint8_t)ip[3]);
  Serial.println(clientID);

  /**** TLS setup ****/
#if defined TLS_INSECURE
  sslClient.setInsecure();
#else
  sslClient.setCACert(ca_cert);
#endif
#if defined TLS_CLIENT_CERT
  sslClient.setCertificate(client_cert);
  sslClient.setPrivateKey(client_key);
#endif
  // Per-read wait (ms). Keep at 0 so client.loop() never blocks waiting for data.
  // (A non-zero value would also override the handshake timeout below.)
  sslClient.setTimeout(0);
  sslClient.setHandshakeTimeout(30);  // seconds, for the whole TLS handshake over RF24

  client.begin(server, MQTT_TLS_PORT, sslClient);
  client.setTimeout(10000);  // ms to wait for broker responses (CONNACK etc.)
  client.setKeepAlive(30);   // seconds
  client.onMessage(messageReceived);
}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;
uint32_t connection_timer = 0;
uint32_t wifi_timer = 0;

void loop() {
  Ethernet.update();

  if (millis() - mesh_timer > 30000) {  //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if (!mesh.checkConnection()) {
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin();
      }
    }
    Serial.println();
  }

  // Every 30 seconds, reconnect WiFi if the connection was lost
  if (WiFi.status() != WL_CONNECTED && millis() - wifi_timer > 30000) {
    wifi_timer = millis();
    connectWiFi();
  }

  // TLS handshakes are slow, so only attempt a (re)connect every 10 seconds
  if (!client.connected() && mesh.mesh_address != MESH_DEFAULT_ADDRESS && millis() - connection_timer > 10000) {
    connect();
    connection_timer = millis();
  }

  client.loop();

  // Every 3 seconds, report to the MQTT server the Node ID of this node
  if (client.connected() && millis() - pub_timer > 3000) {
    pub_timer = millis();
    char msg[32];
    snprintf(msg, sizeof(msg), "Node %u\r\n", (uint8_t)ip[3]);
    client.publish("outTopic", msg);
  }
}
