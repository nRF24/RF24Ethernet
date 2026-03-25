# RF24Ethernet Overview

@tableofcontents

## Overview

The RF24Ethernet library was initially designed as an experiment and potential testing tool for [RF24Network](http://nRF24.github.io/RF24Network), an OSI Layer 3 network driver, allowing a Raspberry Pi to
act as a TCP/IP gateway or host for connected sensor nodes.

Historically, RF24Ethernet used the uIP TCP/IP stack on small AVR-class Arduinos. RF24Ethernet now also supports the newer **lwIP** TCP/IP stack, enabling a more modern IP networking implementation on
more capable MCUs. **By default, lwIP is used on devices with core speed ≥ 50 MHz**, while **uIP remains the legacy/low-memory option** for smaller/slower targets.

An Arduino can interface with any Linux machine or SLIP capable device supporting USB, or
preferably, an RPi runs companion software, [RF24Gateway](http://nRF24.github.io/RF24Gateway/), which creates a network interface linked to the radio network. This interface can be
further linked to the local network or internet. This allows the RPi or Arduino-based gateway to perform automatic discovery and routing of TCP/IP data,
with no required pre-configuration or interaction from the user beyond assigning appropriate addresses to the nodes initially.

## What does it do?

RF24Ethernet creates a network of internet enabled radio-connected sensors and devices. It provides an API and interface very similar to the Arduino Ethernet library,
to allow sensor nodes to connect out to local or internet based devices to retrieve or send information, or be connected to from the internet or your
local network directly.

Sensor nodes can act as individual web servers, simple command-and-control servers, or can connect out as required via TCP/IP.

RF24Ethernet supports both “classic” RF24 Arduino nodes (for example, 8-bit AVR boards using an external **nRF24L01(+)** radio module), and newer nRF52-based boards that can act as RF24Ethernet nodes
directly (for example **Adafruit Feather nRF52840 Express** and **Seeed XIAO nRF52840 Sense**). Compared to older 8-bit + nRF24L01(+) deployments, nRF52x-class devices provide a much more capable MCU
environment and are a natural fit for the **lwIP** networking stack.

## How does it work?

RF24Ethernet provides an IP networking layer on top of the RF24 networking stack:

- On smaller/legacy targets it uses the **uIP** TCP/IP stack.
- On newer/more capable targets it uses the **lwIP** TCP/IP stack (**default when core speed ≥ 50 MHz**).

This allows Arduino-class devices to use a Raspberry Pi running [RF24Gateway](http://nRF24.github.io/RF24Gateway/) (or another supported gateway)
as a bridge to your network or the internet, or simply as a repository for sensor information.

The RF24, RF24Network and RF24Mesh libraries handle the underlying routing, addressing etc. so users do not need to be familiar with the radio modules or libraries.

RF24Network addresses need to be assigned as MAC addresses, and IP addresses can be configured as desired by the user. The master node (00) uses
either the Address Resolution Protocol (ARP) or RF24Mesh to find the appropriate nodes when IP traffic is sent though, and routes traffic to the correct
RF24Network address.

This provides a fairly seamless interaction, since users only need to configure standard IP forwarding and firewall rules as desired.

This table maps the RF24 ecosystem (including RF24Ethernet using **lwIP/uIP** and support for **nRF24L01(+)** and **nRF52x-class** nodes) to the [OSI model](http://en.wikipedia.org/wiki/OSI_model):

| OSI Layer | RF24 Component | C++ Header | Primary Function | Real-World Equivalent |
| :--- | :--- | :--- | :--- | :--- |
| **7. Application** | User Sketch / RF24Mesh | `RF24Mesh.h` | Data generation & node ID management | HTTP, MQTT, DHCP |
| **6. Presentation**| **nrf_to_nrf (CCM)** | `nrf_to_nrf.h` | **Hardware AES-CCM Encryption/Auth** | TLS, AES-GCM, IPsec |
| **5. Session** | **RF24Ethernet (lwIP / uIP)** | `RF24Ethernet.h` | **Socket state & connection management** | BSD Sockets, NetBIOS |
| **4. Transport** | **RF24Ethernet (lwIP / uIP)** | `lwip/tcp.h` or `uip.h` | **TCP/UDP transport & flow control** | TCP, UDP |
| **3. Network** | RF24Network | `RF24Network.h` | Octal routing & IP-over-RF24 encapsulation | IPv4, IPv6, ICMP |
| **2. Data Link** | **nrf_to_nrf** / RF24 Core | `RF24.h` or `nrf_to_nrf.h` | **MAC (Pipes), Auto-ACK, & Framing** | Ethernet (MAC), 802.11 |
| **1. Physical** | nRF24L01+ / nRF52 | **SPI Hardware** | 2.4GHz GFSK Radio Frequency | Fiber, Copper, WiFi PHY |

### TAP vs TUN

RF24Ethernet and RF24Gateway are able to utilize both TAP and TUN type interfaces.

#### TAP

A TAP interface can be looked at much the same as an Ethernet interface, Ethernet headers are used to identify devices via MAC address, and the Address
Resolution Protocol (ARP) is used to perform that identification.

#### TUN

A TUN interface does not utilize Ethernet headers, MAC addresses or ARP, in this case relying on RF24Mesh/IP routing instead.

### Limitations

UDP frames can be up to 512 bytes in length, so UDP/DNS is limited by available memory, and the defined MAX_PAYLOAD_SIZE / UIP_BUFFER_SIZE. If a
DNS request exceeds the maximum allowed size, it will be dropped. DNS responses for addresses such as 'www.google.com' tend to be much smaller than requests
for 'google.com', and will generally work with the default configuration.

Note: the practical limits and tunable values may differ depending on whether you are using the legacy **uIP** stack or the newer **lwIP** stack, and on the capabilities of the target hardware.

### MAC address formatting

RF24Ethernet uses a simple method of formatting the MAC addresses, using the first four bytes to store the characters 'RF24', and the last two bytes to store the
RF24Network address, which makes identification of nodes via MAC address very simple using standard monitoring tools.

| RF24Network Address| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 |
|--------------------|--------|--------|--------|--------|--------|--------|
|      n/a           |   R    |   F    |   2    |   4    |Octal LB|Octal HB|
|      01            |  0x52  |  0x46  |  0x32  |   0x34 |  0x01  |  0x00  |
|      011           |  0x52  |  0x46  |  0x32  |   0x34 |  0x09  |  0x00  |
|      0443          |  0x52  |  0x46  |  0x32  |   0x34 |  0x23  |  0x01  |