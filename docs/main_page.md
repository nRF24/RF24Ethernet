# RF24Ethernet library: TCP/IP over RF24Network

@tableofcontents

## Open & Open-Source IoT Wireless (not WiFi) Networks

**An experiment disconnected...**

<h2 style="text-align: center;">RF24Ethernet - TCP/IP & IoT mesh networking for nrf24l01 and compatible radio modules using Arduino (AVR) and Raspberry Pi/ARM(Linux) devices</h2>

\dot
digraph RF24EthernetOverview {
  graph [
    labelloc="t",
    fontsize=16,
    fontname="Helvetica",
    rankdir="LR",
    splines=ortho,
    nodesep=0.55,
    ranksep=0.7,

    // Dark-theme friendly
    bgcolor="transparent",
    fontcolor="#E6EDF3"
  ];

  node [
    shape=box,
    style="filled",
    fontname="Helvetica",
    fontsize=11,
    margin="0.18,0.12",

    // Global dark-theme defaults
    color="#8B949E",          // border
    fillcolor="#0D1117",      // node background
    fontcolor="#E6EDF3"       // node text
  ];

  // Thick connectors like the original image, but softened for dark theme
  edge [
    dir=none,
    color="#E6EDF3",
    penwidth=10
  ];

  // Palette derived from docs/doxygen-custom.css highlight colors
  // note:     #4493F8
  // remark:   #41B157
  // warning:  #D29922
  // attention:#F85149
  // important:#CBA6FF
  // seealso:  #D633B0

  // --- Top row (3 main blocks) ---
  devices_top [
    label="RF24 Devices/Sensor\nNetwork\n\nArduino, AVR, Raspberry\nPi, ARM/Linux, etc with\nnrf24l01 radio\nmodules",
    width=3.4, height=1.5,
    fillcolor="#1C2A44",      // deep blue-gray base
    color="#4493F8",          // note highlight border
    fontcolor="#EAF4FF"
  ];

  gateway_top [
    label="RF24 Host/Gateway\n\nRaspberry Pi, ARM/Linux\nor PC/Mac via Arduino\nwith nrf24l01 radio\nmodules",
    width=3.4, height=1.5,
    fillcolor="#162E2A",      // deep greenish base
    color="#41B157",          // remark highlight border
    fontcolor="#E6EDF3"
  ];

  internet_top [
    label="Internet & LAN\n\n- Send/Receive\ninformation to/from local\nnetwork or internet based\nservers or systems",
    width=3.4, height=1.5,
    fillcolor="#2B2336",      // deep purple base
    color="#CBA6FF",          // important highlight border
    fontcolor="#E6EDF3"
  ];

  devices_top -> gateway_top;
  gateway_top  -> internet_top;

  // --- Bottom row (3 detail blocks) ---
  // Use a common card background, with borders matching the associated top section.
  devices_bottom [
    label="- Transmit data from any\nArduino or RPi supported\nsensors or devices\n- Act as command & control\nserver\n- Remotely control switches,\nrelays etc\n- Interact with the environment",
    width=3.4, height=1.7,
    fillcolor="#0D1117",
    color="#4493F8",
    fontcolor="#E6EDF3"
  ];

  gateway_bottom [
    label="- Interact with Arduino sensors\nusing standard tools & protocols\nover TCP/IP\n\n- Configure automation of tasks\nusing nodeJS, PHP, or other\nscripting methods, independent\nof RF24 library APIs",
    width=3.4, height=1.7,
    fillcolor="#0D1117",
    color="#41B157",
    fontcolor="#E6EDF3"
  ];

  internet_bottom [
    label="- Connect to local network or\ninternet resources\n- Control remote, wireless\nsensor nodes using a web\nbrowser or scripting\n- Link remote sensor\nnetworks together via SSH,\nVPN etc",
    width=3.4, height=1.7,
    fillcolor="#0D1117",
    color="#CBA6FF",
    fontcolor="#E6EDF3"
  ];

  { rank=same; devices_top; gateway_top; internet_top; }
  { rank=same; devices_bottom; gateway_bottom; internet_bottom; }

  // Vertical connectors
  devices_top  -> devices_bottom;
  gateway_top  -> gateway_bottom;
  internet_top -> internet_bottom;
}
\enddot

## Goals

- Bring the reliability of TCP/IP and potential of mesh networking/IoT to even the smallest of Arduino devices
- Enable self-sustaining wireless sensor networks that seamlessly link together using standard protocols & networking
- Simplify & automate it
- Experiment with/document a model for completely open-source communication

## News

- **2026:** RF24Ethernet now supports the **lwIP** stack. Most devices with processors **≥ 50MHz** will automatically use lwIP.
  - Devices like **ESP32** and **ESP8266** already use **lwIP** internally for WiFi, so the separate **lwIP Arduino library is not required** on those platforms.
  - On other supported platforms, users may need to install **lwIP** using the Arduino Library Manager.
- **2026:** RF24Ethernet now supports direct TCP/IP communication without the need for a Raspberry Pi gateway. Two or more devices running RF24Ethernet can now connect directly.

See the releases' descriptions on
[the library's release page](https://github.com/nRF24/RF24Ethernet/releases) for a list of changes.

See [version history](pre_nrf24_changelog.md) for more about the lineage of RF24Ethernet.

## Configuration and Setup

The hardest part of setting up is getting the first radio module connected properly.

Scroll to the bottom of the [RF24 radio documentation for pin connections](https://nrf24.github.io/RF24/).

Once you have done that, see the [Configuration and Setup](config_setup.md) page for general installation and configuration information.

## Detailed Overview

See the [**Overview**](overview.md) page.

## Building a Network - Customizing your RF24 TCP/IP network

See the [Building a network](custom_networks.md) page.

## Home Automation with MQTT and Node-Red

See the [Project Setup Info](https://create.arduino.cc/projecthub/TMRh20/iot-home-automation-w-nrf24l01-raspberry-pi-9ee904) at Arduino Project Hub.

## About Troubleshooting

The RF24 libraries are divided into layers, generally according to the OSI model, which allows specialized testing and troubleshooting of individual layers.

See the [Troubleshooting section](troubleshooting.md) for general troubleshooting steps.