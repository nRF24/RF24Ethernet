# RF24Ethernet MQTT BearSSL (Arduino UNO Q)

App Lab version of the [MQTT_BearSSL](https://github.com/TMRh20/Sketches/tree/master/RF24Ethernet/Sketches/MQTT_BearSSL) sketch. The MCU connects to an MQTT broker on port 8883 over an RF24Mesh network, using BearSSL for TLS.

The UNO Q MCU has no RTC and no crypto chip, so the Linux side provides both:

| Bridge call    | Returns                                      | Used for                          |
|----------------|----------------------------------------------|-----------------------------------|
| `get_time`     | Unix time in seconds (UTC)                   | TLS certificate date checks       |
| `get_datetime` | `YYYY-MM-DD HH:MM:SS` (UTC)                  | Log line at startup               |
| `get_random`   | 32-bit number from the kernel CSPRNG         | Seeds `random()`, which BearSSL uses |

The MCU re-reads the time every hour to correct `millis()` drift. The Linux clock must be NTP-synced (`timedatectl` shows `System clock synchronized: yes`).

## Hardware

1. nRF24L01 on the UNO header SPI (D11 MOSI, D12 MISO, D13 SCK), CE on D7, CSN on D8.
2. RF24Gateway running on a Linux machine like Raspberry Pi, with a TLS enabled MQTT server running on port 8883 or network connectivity enabled to reach remote MQTT servers. (Forwarding enabled etc.)

## Setup

1. `sketch/certificates.h`: paste the broker CA trust anchor (see the comments in the file) and delete the `#error` line.
2. `sketch/secrets.h`: set the MQTT username and password.
3. In `sketch/sketch.ino`, set `ip`, `gateway` and `server` for your network.

## Notes

- `Arduino_SecureElement` is deliberately not in `sketch.yaml`. It has no UNO Q support and fails with `#error "Board not supported"`. ArduinoBearSSL works without it on boards with no secure element.
- The sketch defines `__real_gettimeofday` to work around an undefined reference in Zephyr core 1.1.0. If a newer core reports a multiple definition, delete that function.
