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
  Basic MQTT over TLS example (BearSSL) - Arduino UNO Q App Lab version

  This sketch is the TLS version of the mqtt_basic example.
  It connects to an MQTT server on port 8883 then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives.
  - it assumes the received payloads are strings not binary
  - Continually publishes its nodeID to the outTopic

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function.

  Board: Arduino UNO Q. The sketch runs on the STM32U585 MCU (Zephyr core).
  The nRF24L01 is on the UNO header SPI (D11 MOSI, D12 MISO, D13 SCK),
  with CE on D7 and CSN on D8.

  The UNO Q MCU has no RTC and no crypto chip, so the date/time and the
  random seed come from the Linux side (python/main.py) over the Bridge.

  MQTT credentials are kept in secrets.h and the broker's CA trust anchor
  in certificates.h. Edit both files before uploading.
*/

#include <Arduino_RouterBridge.h>
#include <sys/time.h>
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

// Re-read the time from the MPU this often, to correct millis() drift
#define TIME_RESYNC_MS 3600000UL

RF24 radio(7, 8); // CE = D7, CSN = D8
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

IPAddress ip(10, 10, 2, 4);
IPAddress gateway(10, 10, 2, 2); // Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);  // The ip of the MQTT server
char clientID[20];

void messageReceived(MQTTClient* client, char topic[], char payload[], int length)
{
    Monitor.println("incoming: ");
    Monitor.print(topic);
    Monitor.print(" - ");
    Monitor.println(payload);
}

EthernetClient ethClient;

// BearSSLClient::stop() sends a TLS close_notify and then waits for the
// broker to send one back. Closing the TCP connection first makes it
// return immediately.
class BearSSLClientQuickStop : public BearSSLClient
{
public:
    BearSSLClientQuickStop(Client& net, const br_x509_trust_anchor* tas, int numTAs)
        : BearSSLClient(net, tas, numTAs), _net(net) {}
    void stop() override
    {
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
// Unix time (UTC) read from the MPU, and the millis() value when it was read
static uint32_t epochAtSync = 0;
static uint32_t millisAtSync = 0;

unsigned long getTime()
{
    return epochAtSync + (millis() - millisAtSync) / 1000;
}

// Zephyr core 1.1.0 wraps gettimeofday() but leaves __real_gettimeofday
// undefined, so time() fails to link. Supply it from the MPU time.
// Remove this if a newer core reports a multiple definition.
extern "C" int __real_gettimeofday(struct timeval* tv, void* tz)
{
    (void)tz;
    if (tv) {
        uint32_t ms = millis() - millisAtSync;
        tv->tv_sec = epochAtSync + ms / 1000;
        tv->tv_usec = (ms % 1000) * 1000;
    }
    return 0;
}

// Ask the MPU for the time. Retry, because Linux boots slower than the MCU.
static bool syncTimeFromMPU(uint32_t timeoutMs)
{
    const uint32_t start = millis();
    do {
        uint32_t t = 0;
        if (Bridge.call("get_time").result(t) && t > 1700000000UL) { // Sanity check: after 2023
            epochAtSync = t;
            millisAtSync = millis();
            return true;
        }
        delay(1000);
    } while (millis() - start < timeoutMs);
    return false;
}

/**** Randomness ****/
// ArduinoBearSSL seeds TLS from random() when no crypto chip is present.
// Seed random() from the Linux kernel CSPRNG so the session keys differ every boot.
static void seedRandom()
{
    uint32_t seed = 0;
    if (!Bridge.call("get_random").result(seed)) {
        Monitor.println("get_random failed, seeding from micros()");
        seed = micros();
    }
    randomSeed(seed);
}

// Add 256 bits from the Linux kernel CSPRNG on every connection.
static void tlsProfile(br_ssl_client_context* sc, br_x509_minimal_context* xc, const br_x509_trust_anchor* tas, size_t numTAs)
{
    br_ssl_client_init_full(sc, xc, tas, numTAs); // Must be first: it clears sc
    uint32_t r[8] = {0};
    for (auto& v : r) {
        if (!Bridge.call("get_random").result(v)) {
            Monitor.println("WARNING: get_random failed, weak TLS keys");
        }
    }
    br_ssl_engine_inject_entropy(&sc->eng, r, sizeof(r));
}
  
void connect()
{
    Monitor.print("connecting (TLS)...");
    const uint32_t clTimeoutStart = millis();
    while (!client.connect(clientID, MQTT_USERNAME, MQTT_PASSWORD)) {
        Monitor.print(".");
        if (millis() - clTimeoutStart > 5001) {
            Monitor.println();
            mesh.renewAddress();
            return;
        }
        const uint32_t timerStart = millis();
        // Instead of delay, keep the RF24 stack updating
        while (millis() - timerStart < 1000) {
            Ethernet.update();
        }
    }

    Monitor.println("\nconnected!");
    client.publish("outTopic", "hello world");
    client.subscribe("inTopic", 2);
}

void setup()
{
    Bridge.begin();
    Monitor.begin();

    if (syncTimeFromMPU(60000)) {
        String dateTime;
        Bridge.call("get_datetime").result(dateTime);
        Monitor.print("Time from MPU (UTC): ");
        Monitor.println(dateTime);
    }
    else {
        Monitor.println("Time sync failed - TLS certificate checks will fail");
    }

    seedRandom();
    ArduinoBearSSL.onGetTime(getTime);

    Ethernet.begin(ip, gateway);

    if (mesh.begin()) {
        Monitor.println(" OK");
    }
    else {
        Monitor.println(" Failed");
    }

    // Convert the last octet of the IP address to an identifier used
    snprintf(clientID, sizeof(clientID), "arduinoClient%u", (uint8_t)ip[3]);
    Monitor.println(clientID);

    sslClient.setProfile(tlsProfile);
    client.begin(server, MQTT_TLS_PORT, sslClient);
    client.setTimeout(30000); // ms to wait for broker responses. The TLS handshake over RF24 is slow
    client.setKeepAlive(30);  // seconds
    client.onMessageAdvanced(messageReceived);
}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;
uint32_t time_timer = 0;

void loop()
{
    Ethernet.update();

    if (millis() - mesh_timer > 30000) { // Every 30 seconds, test mesh connectivity
        mesh_timer = millis();
        if (!mesh.checkConnection()) {
            if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
                mesh.begin();
            }
        }
        Monitor.println();
    }

    if (millis() - time_timer > TIME_RESYNC_MS) { // Every hour, correct the clock from the MPU
        time_timer = millis();
        syncTimeFromMPU(0);
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
