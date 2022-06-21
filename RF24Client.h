
/*
 RF24Client.h - Arduino implementation of a uIP wrapper class.
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
  */

#ifndef RF24CLIENT_H
#define RF24CLIENT_H

#include "Print.h"
#include "Client.h"

//#define UIP_SOCKET_DATALEN UIP_TCP_MSS
//#define UIP_SOCKET_NUMPACKETS UIP_RECEIVE_WINDOW/UIP_TCP_MSS+1
//#ifndef UIP_SOCKET_NUMPACKETS
//#define UIP_SOCKET_NUMPACKETS 5
//#endif

#define UIP_CLIENT_CONNECTED 0x10
#define UIP_CLIENT_CLOSE 0x20
#define UIP_CLIENT_REMOTECLOSED 0x40
#define UIP_CLIENT_RESTART 0x80
#define UIP_CLIENT_STATEFLAGS (UIP_CLIENT_CONNECTED | UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED | UIP_CLIENT_RESTART)
#define UIP_CLIENT_SOCKETS ~UIP_CLIENT_STATEFLAGS

/**
 * @warning <b> This is used internally and should not be accessed directly by users </b>
 */
typedef struct {
    uint8_t state;
    uint8_t packets_in[UIP_SOCKET_NUMPACKETS];
    /** The local TCP port, in network byte order. */
    uint16_t lport;
} uip_userdata_closed_t;


/**
 * Data structure for holding per connection data
 * @warning <b> This is used internally and should not be accessed directly by users </b>
 */
typedef struct {
    bool hold;
    bool sent;
    bool packets_in;
    bool packets_out;
    bool windowOpened;
    uint8_t state;
    uint16_t out_pos;
    uint16_t dataPos;
    uint16_t dataCnt;
#if UIP_CLIENT_TIMER >= 0
     uint32_t timer;
#endif
    uint32_t restartTime;
    uint32_t restartInterval;
    uint32_t connAbortTime;
    uint32_t connectTimeout;
    uint32_t connectTimer;
    uint8_t myData[OUTPUT_BUFFER_SIZE];
    bool initialData;
} uip_userdata_t;


class RF24Client : public Client {

public:

    /** Basic constructor */
    RF24Client();

    /** Establish a connection to a specified IP address and port */
    int connect(IPAddress ip, uint16_t port);

    /**
     * Establish a connection to a given hostname and port
     * @note UDP must be enabled in uip-conf.h for DNS lookups to work
     *
     * @note Tip: DNS lookups generally require a buffer size of 250-300 bytes or greater.
     * Lookups will generally return responses with a single A record if using hostnames like
     * "www.google.com" instead of "google.com" which works well with the default buffer size
     */
    int connect(const char *host, uint16_t port);

    /**
     * Read available data into a buffer
     * @code
     * uint8_t buf[size];
     * client.read(buf,size);
     * @endcode
     */
    int read(uint8_t *buf, size_t size);

    /**
     * Read data one byte at a time
     * @code
     * char c = client.read();
     * @endcode
     */
    int read();

    /** Disconnects from the current active connection */
    void stop();

    /** Indicates whether the client is connected or not */
    uint8_t connected();

    /**
     * Write a single byte of data to the stream
     * @note This will write an entire TCP payload with only 1 byte in it
     */
    size_t write(uint8_t);

    /** Write a buffer of data, to be sent in a single TCP packet */
    size_t write(const uint8_t *buf, size_t size);

    /**
     * Indicates whether data is available to be read by the client.
     * @return Returns the number of bytes available to be read
     * @note Calling client or server available() keeps the IP stack and RF24Network layer running, so needs to be called regularly,
     * even when disconnected or delaying for extended periods.
     */
    int available();

    /**
     * Wait Available
     *
     * Helps to ensure all incoming data has been received, prior to writing data back to the client, etc.
     *
     * Indicates whether data is available to be read by the client, after waiting a maximum period of time.
     * @return Returns the number of bytes available to be read or 0 if timed out
     * @note Calling client or server available() keeps the IP stack and RF24Network layer running, so needs to be called regularly,
     * even when disconnected or delaying for extended periods.
     */
    int waitAvailable(uint32_t timeout=750);

    /** Read a byte from the incoming buffer without advancing the point of reading */
    int peek();

    /** Flush all incoming client data from the current connection/buffer */
    void flush();

    using Print::write;

    operator bool();
    virtual bool operator==(const EthernetClient&);
    virtual bool operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };

    static uip_userdata_t all_data[UIP_CONNS];

private:

    RF24Client(struct uip_conn *_conn);
    RF24Client(uip_userdata_t* conn_data);

    uip_userdata_t* data;

    static int _available(uip_userdata_t *);

    static uip_userdata_t* _allocateData();
    static size_t _write(uip_userdata_t *,const uint8_t *buf, size_t size);

    friend class RF24EthernetClass;
    friend class RF24Server;

    friend void serialip_appcall(void);
    friend void uip_log(char* msg);
};


#endif // RF24CLIENT_H