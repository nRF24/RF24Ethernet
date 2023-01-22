# Version Info

@tableofcontents

## v1.6 - Aug-Dec 2015
- Address problems with stream functions like client.parseInt() or find()
- Tested working with MQTT via [PubSub library](https://github.com/knolleary/pubsubclient)
- Fix: Connection state set before begin allocated
- Workaround for HTTP servers sending half TCP MSS
- Automatically assign mesh nodeID based on IP & update examples
- [ESP8266 (Arduino)](https://github.com/esp8266/Arduino) support

## v1.51RC1 - Apr15-16 2015
- Seemingly stable release candidate
- Major change: [RF24Gateway](http://nRF24.github.io/RF24Gateway/) replaces RF24toTUN
- Now defaults to using RF24Mesh (TUN) interface
- Apr 16 - Use external buffer for uIP to save memory. Requires Updating RF24Network and RF24Mesh.

## v1.5RC1 - Apr15-16 2015
- Seemingly stable release candidate
- Major change: [RF24Gateway](http://nRF24.github.io/RF24Gateway/) replaces RF24toTUN
- Now defaults to using RF24Mesh (TUN) interface
- Apr 16 - Use external buffer for uIP to save memory. Requires Updating RF24Network and RF24Mesh.

## v1.4b/1.411b - March 14 - Apr 7 2015
- Add Ethernet.update() function
- Improve/Fix outgoing data handling
- Fix: Hanging in 1.4b

## v1.3b - March 6 2015
- Major Improvement: Better TCP window handling increases reliability, throughput and performance

## v1.24b - March 3 2015
- Utilize separate incoming/outgoing buffers (bugfix)
- Update documentation for DNS & UDP
- Add waitAvailable() function, update examples to demonstrate usage.

## v1.23b - Jan 22 2015
- Small bugfixes from v1.20
- Slightly reduced latency
- Code clean-up/Reduce code size and memory usage for main Client/Server code
- Cleaned up some examples, added DNS and SimpleServer_Minimal examples

## v1.221b - Jan 16 2015
- Add UDP support
- Add DNS support
- Add support for up to 512 byte buffer size
- Reduce used memory and program space
- Support for multiple connections with per-connection memory  buffers

## v1.1b - Jan 4 2015
- Add connection timeout to recover from hangs during failed client downloads
- Better TCP window management to prevent hangs during client downloads
- Stability improvements

## v1.0b - Dec 2014
- Outgoing client data corruption should be fixed
