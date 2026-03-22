# Configuration and Set-Up

@tableofcontents

RF24Ethernet requires the RF24 and RF24Network libraries (optionally RF24Mesh).

See <http://tmrh20.github.io> for documentation and downloads

See [this video walk-through](https://www.youtube.com/watch?v=eYiWUTjNSuc) of the software setup with Raspberry Pi and Arduino.

## Linux Gateway (Raspberry Pi, etc)

Gateway setup is documented here:

- [Linux Gateway (RF24Gateway + TUN/TAP)](gateway-linux.md)

## Arduino

1. For Arduino devices, use the Arduino Library Manager to install the RF24Ethernet library and all related dependencies
2. For some devices with >=50MHz processors, the lwIP IP stack needs to be installed from the Library Manager as well.
3. Open the included Getting_Started_SimpleServer or Getting_Started_SimpleClient example
4. Configure your chosen CE and CS pins for the radio connection.
5. Configure the IP address according to your preferences, (last octet must == RF24Mesh nodeID) with the gateway set to the chosen IP of the RPi or other device.
6. Pick 1 from these 2 options:

   1. Connect into your nodes web-server at `http://ip-of-your-node:1000` from the RPi or configure the client sketch to connect to a server
running on the Raspberry Pi. Users should also be able to ping the IP of the node from the Raspberry Pi.

   2. Run the examples from the Headless example directory on 2 or more devices.

@note To minimize memory usage on Arduino, edit RF24Network_config.h with a text editor, and uncomment `#define DISABLE_USER_PAYLOADS`. This
will disable standard RF24Network messages, and only allow external data, such as TCP/IP information. Remember to comment for normal operation!

## Non-Raspberry Pi (Linux etc) Devices

Arduino can also function as a gateway for any Linux machine or PC/MAC that supports SLIP.

See the SLIP_Gateway and SLIP_InteractiveServer
examples for usage without the need for a Raspberry Pi.

## Accessing External Systems: Forwarding and Routing

In order to give your network or nodes access to your network or the internet beyond the RPi, it needs to be configured to route traffic
between the networks.

1. Run

   ```shell
   sudo sysctl -w net.ipv4.ip_forward=1
   ```

   to allow the RPi to forward requests between the network interfaces
2. Run

   ```shell
   sudo iptables -t nat -A POSTROUTING -j MASQUERADE
   ```

   to allow the RPi to perform NAT between the network interfaces

> [!note]
> This configuration is generally for initial testing only. Users may also need to add a static route to their local machine, or configure port forwarding on the RPi.
>
> See the following links for more information on configuring and using IPTables:
>
> - <http://www.karlrupp.net/en/computer/nat_tutorial>
> - <http://serverfault.com/questions/326493/basic-iptables-nat-port-forwarding>

<!--  -->

> [!warning]
> **Note:** Users are responsible to manage further routing rules along with their IP traffic in order to prevent unauthorized access.

## Advanced (uIP) Configuration and Info

See the [User Configuration](group__UipConfiguration.html) section for modifying uip-conf.h
