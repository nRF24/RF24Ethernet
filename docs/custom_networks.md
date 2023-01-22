# Building an RF24Ethernet Network

@tableofcontents

The general configuration instructions set up a basic sensor network, with static nodes, using TCP/IP over RF24Network. The layered
library design also allows some nodes to communicate solely at the RF24Network level, creating hybrid networks to extend range over
large distances, or improve connectivity in distant areas.

## RF24Mesh Networks
RF24Mesh allows dynamic configuration of RF24Network addresses, and allows the creation of dynamic sensor networks. Nodes are able
to move around physically, releasing and renewing their RF24Network address as required. Once configured with a unique nodeID, nodes using RF24Mesh
can automatically configure themselves and join the network as required, with no user interaction.

See the included [Getting_Started_SimpleServer_Mesh example](Getting_Started_SimpleServer_Mesh_8ino-example.html) for general usage and configuration information
with RF24Ethernet.

### RF24Mesh - Additional Information
When utilizing RF24Mesh along with RF24Ethernet, relay/routing nodes should be configured with RF24Mesh, or designated on the master node as static nodes
with a pre-configured RF24Network address.

RF24Mesh can be used with TAP/Ethernet configurations, but utilizing TUN/RF24Mesh together will reduce overhead.

See the links below for more information on utilizing RF24Mesh with RF24Ethernet and RF24Network.

## RF24Gateway
See http://nRF24.github.io/RF24Gateway/ for more information on RF24Gateway

## Hybrid- RF24Network/RF24Ethernet Networks
The default configuration of RF24Ethernet and RF24Network enables both TCP/IP and the underlying messaging protocols provided by RF24Network.

Relay/Routing nodes can be configured using only the RF24 and RF24Network layers (optionally RF24Mesh), and will automatically handle data
accordingly, whether it is an external data type (TCP/IP) or an internal network message from another RF24Network node. This allows very small and
inexpensive devices such as ATTiny to act as relay/routing nodes when extending network range and/or connectivity.

Arduino/AVR devices can utilize custom or included sleep modes in RF24Network, and benefit from the lower power consumption and low-power features
of the nrf24l01 chips directly.

## Links
See http://nRF24.github.io/RF24Mesh for more information on using RF24Mesh to create a dynamic network, with automatic RF24Network addressing <br>
See http://nRF24.github.io/RF24Network for more information on using RF24Network as well as addressing and topology.
