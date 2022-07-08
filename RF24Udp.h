/*
 RF24Udp.h - Arduino implementation of a uIP wrapper class.
 Copyright (c) 2014 tmrh20@gmail.com, github.com/TMRh20
 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Updated for RF24Ethernet, TMRh20 2014-2015
 */

#ifndef UIPUDP_H
#define UIPUDP_H

#include "RF24Ethernet.h"

#if UIP_CONF_UDP > 0

    #include <Udp.h>

    #define UIP_UDP_MAXDATALEN    1500
    #define UIP_UDP_PHYH_LEN      UIP_LLH_LEN + UIP_IPUDPH_LEN
    #define UIP_UDP_MAXPACKETSIZE UIP_UDP_MAXDATALEN + UIP_UDP_PHYH_LEN

typedef struct
{
    uint16_t out_pos;
    uint8_t packet_next;
    uint8_t packet_in;
    uint8_t packet_out;
    int packet_in_size;
    uint16_t in_pos;
    boolean send;
} uip_udp_userdata_t;

class RF24UDP : public UDP
{
private:
    struct uip_udp_conn* _uip_udp_conn;

    uip_udp_userdata_t appdata;

public:
    /** @brief Constructor */
    RF24UDP();

    /**
     * @brief initialize, start listening on specified port.
     * @returns 1 if successful, 0 if there are no sockets available to use
     */
    uint8_t begin(uint16_t);

    /** @brief Finish with the UDP socket */
    void stop();

    /**
     * @brief Sending UDP packets
     *
     * Start building up a packet to send to the remote host specific in ip and port
     * @return 1 if successful, 0 if there was a problem with the supplied IP address or port
     */
    int beginPacket(IPAddress ip, uint16_t port);

    /**
     * Start building up a packet to send to the remote host specific in host and port
     * @return 1 if successful, 0 if there was a problem resolving the hostname or port
     */
    int beginPacket(const char* host, uint16_t port);

    /**
     * @brief Finish off this packet and send it
     * @return 1 if the packet was sent successfully, 0 if there was an error
     */
    int endPacket();

    /** @brief Write a single byte into the packet */
    size_t write(uint8_t);

    /** @brief Write @p size bytes from @p buffer into the packet */
    size_t write(const uint8_t* buffer, size_t size);

    using Print::write;

    /**
     * Start processing the next available incoming packet
     * @return The size of the packet in bytes, or 0 if no packets are available
     */
    int parsePacket();

    /** Number of bytes remaining in the current packet */
    int available();

    /** Read a single byte from the current packet */
    int read();

    /**
     * Read up to @p len bytes from the current packet and place them into @p buffer
     * @return The number of bytes read, or 0 if none are available
     */
    int read(unsigned char* buffer, size_t len);

    /**
     * Read up to @p len characters from the current packet and place them into @p buffer
     * @return The number of characters read, or 0 if none are available
     */
    int read(char* buffer, size_t len)
    {
        return read((unsigned char*)buffer, len);
    };

    /** @return The next byte from the current packet without moving on to the next byte */
    int peek();

    /** < Finish reading the current packet  */
    void flush();

    /** Return the IP address of the host who sent the current incoming packet */
    IPAddress remoteIP();

    /** Return the port of the host who sent the current incoming packet */
    uint16_t remotePort();

private:
    friend void uipudp_appcall(void);

    friend class RF24EthernetClass;
    friend class RF24Client;
    static void _send(uip_udp_userdata_t* data);
};

#endif // UDP Enabled
#endif // UIPUDP_H
