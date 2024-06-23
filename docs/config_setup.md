# Configuration and Set-Up

@tableofcontents

RF24Ethernet requires the RF24 and RF24Network libraries (optionally RF24Mesh) <br>
See http://tmrh20.github.io for documentation and downloads

See [this video walk-through](https://www.youtube.com/watch?v=eYiWUTjNSuc) of the software setup with Raspberry Pi and Arduino.

### Raspberry Pi
On the Raspberry Pi, a companion program, <a href="http://nRF24.github.io/RF24Gateway/">RF24Gateway</a> must be installed along with the RF24 and RF24Network libraries
1. Enter the following to download an install script that will build and install the needed RF24* libraries:
   ```shell
   wget https://raw.githubusercontent.com/nRF24/.github/main/installer/install.sh
   chmod +x install.sh
   ./install.sh
   ```
2. Next, build and run the [RF24Gateway_ncurses example](https://nrf24.github.io/RF24Gateway/RF24Gateway_ncurses_8cpp-example.html)
   ```shell
   cd rf24libs/RF24Gateway/examples/build
   make
   cd ncurses
   ./RF24Gateway_ncurses
   ```
3. The application will require the user to specify an IP address and Subnet Mask: 10.10.2.2 and 255.255.255.0 are the defaults with RF24Ethernet examples, but this only works when running as root. Run the following commands to configure the interface and IP, where `pi` is your username:
   ```shell
   sudo ip tuntap add dev tun_nrf24 mode tun user pi multi_queue
   sudo ifconfig tun_nrf24 10.10.2.2/24
   ```
4. Raspberry Pi defaults to the master node (00) using RF24Mesh. Secondary Raspberry pi nodes need to specify their RF24Network address or RF24Mesh nodeID.

### Arduino
1. For Arduino devices, use the Arduino Library Manager to install the RF24Ethernet library and all related dependencies
2. Open the included Getting_Started_SimpleServer or Getting_Started_SimpleClient example
3. Configure your chosen CE and CS pins for the radio connection.
4. Configure the IP address according to your preferences, (last octet must == RF24Mesh nodeID) with the gateway set to the chosen IP of the RPi.
5. Connect into your nodes web-server at `http://ip-of-your-node:1000` from the RPi or configure the client sketch to connect to a server
running on the Raspberry Pi. Users should also be able to ping the IP of the node from the Raspberry Pi.

@note To minimize memory usage on Arduino, edit RF24Network_config.h with a text editor, and uncomment `#define DISABLE_USER_PAYLOADS`. This
will disable standard RF24Network messages, and only allow external data, such as TCP/IP information. Remember to comment for normal operation!

### Non-Raspberry Pi (Linux etc) Devices
Arduino can also function as a gateway for any Linux machine or PC/MAC that supports SLIP. <br>
See the SLIP_Gateway and SLIP_InteractiveServer
examples for usage without the need for a Raspberry Pi.

### Accessing External Systems: Forwarding and Routing
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

@note This configuration is generally for initial testing only. Users may also need to add a static route to their local machine, or configure port forwarding on the RPi.<br>
See the following links for more information on configuring and using IPTables:<br>
http://www.karlrupp.net/en/computer/nat_tutorial<br>
http://serverfault.com/questions/326493/basic-iptables-nat-port-forwarding

@warning **Note:** Users are responsible to manage further routing rules along with their IP traffic in order to prevent unauthorized access.

## Advanced (uIP) Configuration and Info
See the [User Configuration](group__UipConfiguration.html) section for modifying uip-conf.h
