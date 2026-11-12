# MQTT over TLS (BearSSL) for RF24Ethernet

This is the secure (TLS) version of the `mqtt_basic` example. It connects to an
MQTT broker over an encrypted connection, using the RF24 mesh network as its
link.

```
MQTTClient -> BearSSLClient (TLS) -> EthernetClient (RF24Ethernet) -> RF24Mesh
```

## What it does

Once connected to the broker on port 8883, the sketch:

- publishes `hello world` to `outTopic`
- subscribes to `inTopic` and prints any messages it receives to Serial
- publishes `Node <id>` to `outTopic` every 3 seconds, where `<id>` is the
  last number of the node's IP address
- checks its mesh connection every 30 seconds and reconnects if needed

## Required libraries

Install these from the Arduino Library Manager:

- **MQTT** by Joel Gaehwiler
- **ArduinoBearSSL** by Arduino
- **RF24**, **RF24Network**, **RF24Mesh**, **RF24Ethernet**

Don't install the OPEnSLab **SSLClient** library alongside ArduinoBearSSL.
Both contain BearSSL, and the build will fail with duplicate symbol errors.

## Files

| File | What it holds |
|------|---------------|
| `MQTT_BearSSL.ino` | The sketch |
| `secrets.h` | Your MQTT username and password |
| `certificates.h` | The broker's CA certificate, converted for BearSSL |

## Setup

### 1. Set your MQTT username and password

Edit `secrets.h`:

```cpp
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
```

### 2. Add your CA certificate

BearSSL can't read a normal `.crt` (PEM) file. You have to convert the CA
certificate that signed your broker's certificate into C code first. Use either
tool:

```sh
# BearSSL's own tool (https://bearssl.org)
brssl ta /etc/mosquitto/certs/ca.crt

# or OPEnSLab's Python tool
python3 pycert_bearssl.py convert --no-search /etc/mosquitto/certs/ca.crt
```

Paste the output into `certificates.h` between the `BEGIN` and `END` markers,
and delete the `#error` line. The array must be named `TAs` and the count
`TAs_NUM`.

### 3. Check the settings in the sketch

| Setting | Default | Notes |
|---------|---------|-------|
| `RF24 radio(A3, A4)` | CE = A3, CSN = A4 | Change to match your wiring |
| `ip` | `10.10.2.4` | This node's IP address. Give each node a different one |
| `gateway` / `server` | `10.10.2.2` | Your RF24 gateway and MQTT broker |
| `MQTT_TLS_PORT` | `8883` | The broker's TLS port |
| `ENTROPY_PIN` | `PA0` | Leave this analog pin unconnected |
| `BUILD_UTC_OFFSET_HOURS` | `-6` | Your computer's time zone offset from UTC |

## Broker setup (Mosquitto)

Add a TLS listener to your Mosquitto config:

```
listener 8883
cafile   /etc/mosquitto/certs/ca.crt
certfile /etc/mosquitto/certs/server.crt
keyfile  /etc/mosquitto/certs/server.key
```

## Things to know

- **The clock.** TLS checks that certificates aren't expired, so the board
  needs to know the date. This sketch uses the time you compiled it plus the
  board's uptime. That works as long as your certificates were valid when you
  compiled. For a node that runs for months, use NTP or an RTC instead. Set
  `BUILD_UTC_OFFSET_HOURS` correctly or the time will be off by hours.
- **Random numbers.** TLS needs randomness for its keys. Boards like the STM32
  Black Pill have no hardware random number generator, so the sketch seeds one
  from noise on a floating analog pin (`ENTROPY_PIN`). Leave that pin
  unconnected.
- **Connecting by IP.** Because the sketch connects to an IP address rather
  than a hostname, BearSSL doesn't check the name on the broker's certificate.
  It still checks that the certificate was signed by your CA.
- **Slow first connection.** The TLS handshake sends several KB of data over
  the radio link, so connecting can take a number of seconds. This is normal.

## Troubleshooting

- **Build error "Paste your generated trust anchor..."**: you haven't finished
  step 2 yet.
- **Connection keeps failing**: check that the username and password match the
  broker, that `certificates.h` was generated from the CA that signed the
  broker's certificate, and that `BUILD_UTC_OFFSET_HOURS` is correct.
- **Nothing happens at all**: check the radio wiring and that the RF24 gateway
  is running at `10.10.2.2`.
