# RF24Ethernet library: TCP/IP over RF24Network

@tableofcontents


## Open & Open-Source IoT Wireless (not WiFi) Networks
**An experiment disconnected...**

![
    @image html images/RF24EthernetOverviewImg.jpg width=70% height=70%
    ](https://github.com/nRF24/RF24Ethernet/blob/master/images/RF24EthernetOverviewImg.jpg)

## Goals
- Bring the reliability of TCP/IP and potential of mesh networking/IoT to even the smallest of Arduino devices
- Enable self-sustaining wireless sensor networks that seamlessly link together using standard protocols & networking
- Simplify & automate it.
- Experiment with/document a model for completely open & open-source communication

## News
See the releases' descriptions on
[the library's release page](http://github.com/nRF24/RF24Ethernet/releases) for a list of
changes.

See [version history](pre_nrf24_changelog.md) for more about the lineage of RF24Ethernet.

## Configuration and Setup
The hardest part of setting up is getting the first radio module connected properly. <br>
Scroll to the bottom of the [RF24 radio documentation for pin connections](http://nRF24.github.io/RF24/) <br><br>
Once you have done that, see the [Configuration and Set-Up](config_setup.md) page for general installation and configuration information

## Detailed Overview
See the [**Overview**](overview.md) page

## Building a Network - Customizing your RF24 TCP/IP network
See the [Building a network](custom_networks.md) page

## Home Automation with MQTT and Node-Red
See the [Project Setup Info](https://create.arduino.cc/projecthub/TMRh20/iot-home-automation-w-nrf24l01-raspberry-pi-9ee904) at Arduino Project Hub

## About Troubleshooting
The RF24 libraries are divided into layers, generally according to the OSI model, which allows specialized testing and troubleshooting of individual layers.

See the [Troubleshooting section](troubleshooting.md) for general troubleshooting steps.
