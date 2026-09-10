/*
 RF24Server.h - Arduino implementation of a uIP wrapper class.
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

#ifndef RF24SERVER_H
#define RF24SERVER_H

#if __has_include(<Server.h>)
    #include <Server.h>
#elif __has_include("Server.h")
    #include "Server.h"
#else
    #include <Arduino.h>
#endif

#include "RF24Client.h"
#include "ethernet_comp.h"

class RF24Server : public Server
{

public:
    RF24Server(uint16_t);
    RF24Server();
    RF24Client available();
    void begin();
#if defined(ESP32)
    /* on esp32 this is a pure virtual func */
    void begin(uint16_t port);
#endif
    size_t write(uint8_t);
    size_t write(const uint8_t* buf, size_t size);
    using Print::write;

    /**
     * Set server side timeouts in mS. If data is not succesfully sent or received in this timeframe, disconnect the client.
     */
    void setTimeout(uint32_t timeout);

    static bool connectionActive;
    static int serverSocket;

private:
#if USE_LWIP < 1
    uint16_t _port;
#elif USE_LWIP == 1
    static struct tcp_pcb* sPcb;
    static struct tcp_pcb* bindPcb;
    static uint16_t _port;
    static EthernetClient::ConnectState* serverState;
#elif USE_LWIP == 2
    static uint16_t _port;
    static bool serverListening;

    static RF24Client serverClient;
#endif
};

#endif
