/*
 *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2026

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://nRF24.github.io/RF24Ethernet/

 *************************************************************************

 **** EXAMPLE REQUIRES: ***
   - Arduino MQTT library: https://github.com/256dpi/arduino-mqtt/
     Shown in Arduino Library Manager as 'MQTT' by Joel Gaehwiler
   - ArduinoBearSSL: https://github.com/arduino-libraries/ArduinoBearSSL
     Shown in Arduino Library Manager as 'ArduinoBearSSL' by Arduino

 *************************************************************************
  Basic MQTT over TLS example (BearSSL)

  This sketch is the TLS version of the mqtt_basic example.
  It connects to an MQTT server on port 8883 then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives.
  - it assumes the received payloads are strings not binary
  - Continually publishes its nodeID to the outTopic

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function.

  MQTT credentials are kept in secrets.h and the broker's CA trust anchor
  in certificates.h. Edit both files before uploading.

  Certificate validity is checked against getTime() below, which uses the
  sketch build time plus uptime. Replace it with NTP or an RTC for
  long-running deployments.
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <ArduinoBearSSL.h>
#include <MQTT.h>
#include "secrets.h"
#include "certificates.h"

#define MQTT_TLS_PORT 8883

// Leave this analog pin unconnected. Its noise is used to seed the RNG.
#define ENTROPY_PIN PA0

// __TIME__ is the build machine's local time. Set its offset from UTC in hours
// (e.g. -6 for CST) so getTime() returns UTC.
#define BUILD_UTC_OFFSET_HOURS -6

RF24 radio(A3, A4);  // CE, CSN
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

IPAddress ip(10, 10, 2, 4);
IPAddress gateway(10, 10, 2, 2);  //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);   //The ip of the MQTT server
char clientID[20];

void messageReceived(MQTTClient* client, char topic[], char payload[], int length) {
  Serial.println("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(payload);
}

EthernetClient ethClient;

// BearSSLClient::stop() sends a TLS close_notify and then waits for the
// broker to send one back. Closing the TCP connection first makes it
// return immediately.
class BearSSLClientQuickStop : public BearSSLClient {
public:
  BearSSLClientQuickStop(Client& net, const br_x509_trust_anchor* tas, int numTAs)
    : BearSSLClient(net, tas, numTAs), _net(net) {}
  void stop() override {
    _net.stop();
    BearSSLClient::stop();
  }
private:
  Client& _net;
};

// TLS layer on top of the RF24Ethernet TCP client, verified against TAs from certificates.h
BearSSLClientQuickStop sslClient(ethClient, TAs, (size_t)TAs_NUM);
MQTTClient client;

/**** Time for certificate validation ****/
// Unix time of the build, from __DATE__ ("Nov  2 2026") and __TIME__ ("18:08:00")
static unsigned long buildTime() {
  static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  const char* d = __DATE__;
  const char* t = __TIME__;
  int month = 1;
  while (month < 12 && strncmp(months + (month - 1) * 3, d, 3) != 0) {
    month++;
  }
  int day = atoi(d + 4);
  int year = atoi(d + 7);
  // Days since 1970-01-01 (civil-from-days inverse, valid for 1970+)
  int y = year - (month <= 2);
  int era = y / 400;
  int yoe = y - era * 400;
  int doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
  int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  long days = (long)era * 146097 + doe - 719468;
  return (unsigned long)days * 86400UL + atoi(t) * 3600UL + atoi(t + 3) * 60UL + atoi(t + 6);
}

unsigned long getTime() {
  return buildTime() - (long)BUILD_UTC_OFFSET_HOURS * 3600L + millis() / 1000;
}

/**** Randomness ****/
// With no crypto chip, ArduinoBearSSL makes its TLS entropy with random(),
// which has only a 32-bit seed. tlsProfile() adds 32 bytes of our own on
// every connection, made by hashing ADC noise and timing jitter.
// The STM32F411 has no hardware RNG, so this is a best effort: its strength
// depends on how noisy ENTROPY_PIN is.
static void gatherEntropy(uint8_t out[32]) {
  br_sha256_context ctx;
  br_sha256_init(&ctx);
  for (int i = 0; i < 1024; i++) {
    uint32_t sample[4] = { (uint32_t)analogRead(ENTROPY_PIN), 0, 0, micros() };
#if defined(ATEMP)
    sample[1] = analogRead(ATEMP);
#endif
#if defined(AVREF)
    sample[2] = analogRead(AVREF);
#endif
    br_sha256_update(&ctx, sample, sizeof(sample));
  }
  br_sha256_out(&ctx, out);
}

// Called by BearSSLClient at the start of every TLS connection
static void tlsProfile(br_ssl_client_context* sc, br_x509_minimal_context* xc,
                       const br_x509_trust_anchor* tas, size_t numTAs) {
  br_ssl_client_init_full(sc, xc, tas, numTAs);  // Must be first: it clears sc
  uint8_t entropy[32];
  gatherEntropy(entropy);
  br_ssl_engine_inject_entropy(&sc->eng, entropy, sizeof(entropy));
}

// Seed random() too, for anything else that uses it
static void seedRandom() {
  uint8_t entropy[32];
  uint32_t seed;
  gatherEntropy(entropy);
  memcpy(&seed, entropy, sizeof(seed));
  randomSeed(seed);
}

void connect() {
  Serial.print("connecting (TLS)...");
  const uint32_t clTimeoutStart = millis();
  while (!client.connect(clientID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    if (millis() - clTimeoutStart > 5001) {
      Serial.println();
      mesh.renewAddress();
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

  seedRandom();
  ArduinoBearSSL.onGetTime(getTime);

  Ethernet.begin(ip, gateway);

  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  //Convert the last octet of the IP address to an identifier used
  snprintf(clientID, sizeof(clientID), "arduinoClient%u", (uint8_t)ip[3]);
  Serial.println(clientID);

  sslClient.setProfile(tlsProfile);
  client.begin(server, MQTT_TLS_PORT, sslClient);
  client.setTimeout(30000);  // ms to wait for broker responses. The TLS handshake over RF24 is slow
  client.setKeepAlive(30);   // seconds
  client.onMessageAdvanced(messageReceived);
}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;

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
  if (!client.connected() && mesh.mesh_address != MESH_DEFAULT_ADDRESS) {
    connect();
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
