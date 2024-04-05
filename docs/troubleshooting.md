# Troubleshooting

@tableofcontents

## Installation & Compilation Errors:

### RPi
Report issues with the installer at https://github.com/TMRh20/RF24/issues

### Arduino
Ensure proper library installation.
- Download and install RF24, RF24Network, RF24Mesh and RF24Ethernet libraries from http://tmrh20.github.io
- See http://arduino.cc/en/Guide/Libraries ( Note: Library folders may need to be renamed ie: Change RF24Network-Development to RF24Network )

## Test hardware configuration

### RPi
1. Navigate to the rf24libs/RF24/examples_RPi folder
2. Configure the correct pins in gettingstarted.cpp (See [general Linux/RPi setup](https://nRF24.github.io/RF24/md_docs_2rpi__general.html))
<!-- http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939) -->
3. Build the examples: `make`
4. Run an example `sudo ./gettingstarted`

### Arduino
1. Open the GettingStarted example sketch (File > Examples > RF24 > GettingStarted)
2. Configure the appropriate CE and CS pins in the example as desired (`RF24 radio(7, 8);`)
3. Configure the assigned radio number in the example
4. Upload to Arduino to test

## Testing RF24Gateway and RF24Ethernet
@note Troubleshooting steps assume a fresh library install with the default configuration, using RF24Mesh/TUN
@warning The maximum payload size configured in RF24Network_config.h will determine the maximum size of TCP or UDP segments. Set to 1514 (TAP) or 1500 (TUN) on Raspberry Pi/Linux devices for full TCP/IP capabilities. TCP+IP+LL headers add 54 bytes of overhead to each payload with TAP/Ethernet, and 40 bytes with TUN/RF24Mesh

### RPi (RF24Gateway)
@see [Documentation for RF24Gateway](http://nRF24.github.io/RF24Gateway/)
1. Run the included RF24Gateway_ncurses example `sudo ./RF24Gateway_ncurses`
2. Test connectivity: `ping 10.10.3.<IP_TO_PING-last-octet>`
3. If connectivity fails, ensure IP information is accurate. Forwarding is required if pinging from a device other than the RPi.
4. If using pins connections other than the default, edit the RF24 constructor in the example file: ie: `RF24 radio(22, 0);`
5. Optional: For RPi to RPi communication over RF24, edit the rf24libs/RF24Network/RF24Network_config.h file. Set `#define MAX_PAYLOAD_SIZE 1514`

### Arduino (RF24Ethernet)
1. Update all RF24 libraries (RF24, RF24Network, RF24Mesh, RF24Ethernet) http://tmrh20.github.io
2. Open, configure and upload one of the included examples to Arduino. (File > Examples > RF24Ethernet)
3. Ensure example hardware and pin configuration matches CE and CS pins selected in step 2
4. If switching between TAP(Ethernet) & TUN(RF24Mesh), ensure the LLHL is defined appropriately in RF24Ethernet/uip-conf.h: `#define UIP_CONF_LLH_LEN 14`
