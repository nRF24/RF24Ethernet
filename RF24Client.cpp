/*
 RF24Client.cpp - Arduino implementation of a uIP wrapper class.
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
#include "RF24Ethernet.h"

#if USE_LWIP == 2
    #include <zephyr/kernel.h>
    #include <zephyr/net/socket.h>
    #include <errno.h>
    #include <stdio.h>
    #include <string.h>
    #include <fcntl.h>
    #include <zephyr/net/net_if.h>
    #include <zephyr/posix/sys/ioctl.h>
    #ifdef __cplusplus
extern "C" {
    #endif

struct net_if* rf24_netif_get_iface(void);

    #ifdef __cplusplus
}

    #endif

RF24Client* RF24Client::g_rf24client_instance = nullptr;
int RF24Client::_socket;
uint32_t RF24Client::serverConnectionTimeout;
uint8_t RF24Client::peekBuffer[64];

#endif
#if USE_LWIP < 1

    #define UIP_TCP_PHYH_LEN UIP_LLH_LEN + UIP_IPTCPH_LEN
uip_userdata_t RF24Client::all_data[UIP_CONNS];

#elif USE_LWIP == 1
// #define LWIP_ERR_T uint32_t

    //
    #if !defined ETHERNET_USING_LWIP_ARDUINO
        #include <lwip/tcp.h>
        #include "lwip/tcpip.h"
        #include "lwip/timeouts.h"
    #else
        #include "lwip/include/lwip/tcp.h"
        #include "lwip/include/lwip/tcpip.h"
    #endif

    #include "RF24Ethernet.h"
/** \cond */
RF24Client::ConnectState* RF24Client::gState[2];
char* RF24Client::incomingData[2];
uint16_t RF24Client::dataSize[2];
struct tcp_pcb* RF24Client::myPcb;
uint32_t RF24Client::clientConnectionTimeout;
uint32_t RF24Client::serverConnectionTimeout;
uint32_t RF24Client::simpleCounter;
bool RF24Client::activeState;
int32_t RF24Client::accepts;

/***************************************************************************************************/

// Called when the remote host acknowledges receipt of data
err_t RF24Client::sent_callback(void* arg, struct tcp_pcb* tpcb, u16_t len)
{

    ConnectState* state = (ConnectState*)arg;
    if (state != nullptr) {
        state->serverTimer = millis();
        state->clientTimer = millis();
        IF_ETH_DEBUG_L1(Serial.println("Client: Sent cb"););

        if (state->dataSentSize >= len) {
            state->dataSentSize -= len;
        }
        else {
            state->dataSentSize = 0;
        }

        if (state->dataSentSize == 0) {
            state->waiting_for_ack = false;
        }
        else {
            state->waiting_for_ack = true;
        }
        state->result = ERR_OK;
    }

    return ERR_OK;
}

/***************************************************************************************************/

err_t RF24Client::blocking_write(struct tcp_pcb* fpcb, ConnectState* fstate, const char* data, size_t len)
{

    if (fpcb == nullptr) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Client: Tx with no fpcb"););
        return ERR_CLSD;
    }

    if (!fstate->connected) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Client: Tx with no connection"););
        return ERR_CLSD;
    }

    const uint32_t timeoutStart = millis();

    if (len > tcp_sndbuf(fpcb)) {
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_APPLY_LOCK();
        }
    #endif
        tcp_output(fpcb);
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_REMOVE_LOCK();
        }
    #endif
        fstate->waiting_for_ack = true;
    }

    while (len > tcp_sndbuf(fpcb)) {
        Ethernet.update();
        if (millis() - timeoutStart > serverConnectionTimeout) {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Client: TCP Send Buffer full"););
            return ERR_BUF;
        }
    }

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if (Ethernet.useCoreLocking) {
        ETHERNET_APPLY_LOCK();
    }
    #endif

    err_t err = ERR_CLSD;
    if (fpcb != nullptr) {
        err = tcp_write(fpcb, data, len, TCP_WRITE_FLAG_COPY);
    }

    if (err != ERR_OK) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Client: BLK Write fail 2: "); Serial.println((int)err););

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_REMOVE_LOCK();
        }
    #endif
        return err;
    }

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if (Ethernet.useCoreLocking) {
        ETHERNET_REMOVE_LOCK();
    }
    #endif

    const uint32_t timerStart = millis();
    while (fstate != nullptr && fstate->waiting_for_ack) {
        if (!fstate->connected) {
            return ERR_CLSD;
        }
        if (millis() - timerStart > 5000) {
            if (fstate != nullptr) {
                return ERR_CLSD;
            }
            break;
        }
        Ethernet.update();
    }

    return ERR_OK;
}

/***************************************************************************************************/

void RF24Client::error_callback(void* arg, err_t err)
{

    ConnectState* state = (ConnectState*)arg;
    if (state != nullptr) {
        state->result = err;
        state->connected = false;
        state->waiting_for_ack = false;
        dataSize[state->stateActiveID] = 0;
        if (state->stateActiveID == activeState) {
            myPcb = nullptr;
        }
    }
    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Client: Err cb: "); Serial.println((int)err););
}

/***************************************************************************************************/

err_t RF24Client::srecv_callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr) {
        state->serverTimer = millis();
    }

    if (p == nullptr) {
        if (state != nullptr) {
            state->connected = false;
        }
        if (tpcb != nullptr) {
            if (tcp_close(tpcb) != ERR_OK) {
                tcp_abort(tpcb);
                tpcb = nullptr;
                if (state->stateActiveID == activeState) {
                    myPcb = nullptr;
                }
                return ERR_ABRT;
            }
            tpcb = nullptr;
            if (state->stateActiveID == activeState) {
                myPcb = nullptr;
            }
        }
        return ERR_OK;
    }
    if (err != ERR_OK || state == nullptr) {
        if (p)
            pbuf_free(p);
        return ERR_OK;
    }

    bool id = state->stateActiveID;
    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Copy data to "); Serial.println(state->stateActiveID););

    struct pbuf* q = p;

    uint32_t timeout = millis();
    while (q != nullptr) {
        if (millis() - timeout > 3000) {
            break;
        }
        const uint8_t* data = static_cast<const uint8_t*>(q->payload);
        if (dataSize[id] + q->len < INCOMING_DATA_SIZE) {
            memcpy(&incomingData[id][dataSize[id]], data, q->len);
            dataSize[id] += q->len;
        }
        else {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Server: srecv - Out of incoming buffer space"););
        }
        q = q->next;
    }

    if (tpcb != nullptr) {
        tcp_recved(tpcb, p->tot_len);
    }
    if (p) {
        pbuf_free(p);
    }
    return ERR_OK;
}

/***************************************************************************************************/

err_t RF24Client::recv_callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{

    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Client: Recv cb"););

    ConnectState* state = (ConnectState*)arg;
    if (p == nullptr) {
        if (state != nullptr) {
            state->connected = false;
        }
        if (tpcb != nullptr) {
            if (tcp_close(tpcb) != ERR_OK) {
                tcp_abort(tpcb);
                tpcb = nullptr;
                if (state->stateActiveID == activeState) {
                    myPcb = nullptr;
                }
                return ERR_ABRT;
            }
            tpcb = nullptr;
            if (state->stateActiveID == activeState) {
                myPcb = nullptr;
            }
        }
        return err;
    }
    if (err != ERR_OK || state == nullptr) {
        if (p)
            pbuf_free(p);

        return err;
    }

    if (state != nullptr) {
        state->clientTimer = millis();
    }

    bool id = state->stateActiveID;
    struct pbuf* q = p;
    uint32_t timeout = millis();
    while (q != nullptr) {
        if (millis() - timeout > 3000) {
            break;
        }
        const uint8_t* data = static_cast<const uint8_t*>(q->payload);
        if (dataSize[id] + q->len < INCOMING_DATA_SIZE) {
            memcpy(&incomingData[id][dataSize[id]], data, q->len);
            dataSize[id] += q->len;
        }
        else {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Client: recv - Out of incoming buffer space"););
        }
        q = q->next;
    }

    if (tpcb != nullptr) {
        tcp_recved(tpcb, p->tot_len);
    }
    if (p) {
        pbuf_free(p);
    }
    return ERR_OK;
}

/***************************************************************************************************/

//void RF24Client::setConnectionTimeout(uint32_t timeout)
//{

//    clientConnectionTimeout = timeout;
//}

/***************************************************************************************************/

err_t RF24Client::clientTimeouts(void* arg, struct tcp_pcb* tpcb)
{

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr) {
        if (millis() - state->clientTimer > state->cConnectionTimeout) {
            if (tpcb->state == ESTABLISHED || tpcb->state == SYN_SENT || tpcb->state == SYN_RCVD) {
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Client: Closed Client PCB TIMEOUT"););
                err_t err = tcp_close(tpcb);
                state->result = err;
                state->connected = false;
            }
        }
    }
    return ERR_OK;
}

/***************************************************************************************************/

err_t RF24Client::serverTimeouts(void* arg, struct tcp_pcb* tpcb)
{

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr && tpcb != nullptr) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Stimeout cb "); Serial.println(millis() - state->serverTimer););

        state->result = ERR_OK;

        if (millis() - state->serverTimer > state->sConnectionTimeout) {
            //if (tpcb->state == ESTABLISHED || tpcb->state == SYN_SENT || tpcb->state == SYN_RCVD) {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Server: Closed Server PCB TIMEOUT "););

            state->result = tcp_close(tpcb);
            state->closeTimer = millis();
            dataSize[activeState] = 0;
            state->connected = false;
            if (state->result != ERR_OK) {
                tcp_arg(tpcb, nullptr);
                tcp_abort(tpcb);
                tpcb = nullptr;
                tcp_arg(myPcb, nullptr);
                myPcb = nullptr;
                return ERR_ABRT;
            }
            myPcb = nullptr;
            return state->result;

            // }
        }
        return state->result;
    }
    return ERR_CLSD;
}

/***************************************************************************************************/

err_t RF24Client::closed_port(void* arg, struct tcp_pcb* tpcb)
{

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Client Poll Cb ID: "); Serial.println(state->identifier));
    }

    if (myPcb == nullptr) {
        if (state != nullptr && tpcb != nullptr) {

            if ((tpcb->state == ESTABLISHED || tpcb->state == SYN_SENT || tpcb->state == SYN_RCVD)) {
                if (state->backlogWasAccepted == false) {

                    state->backlogWasAccepted = true;
                    state->connectTimestamp = millis();
                    state->connected = true;
                    accepts--;
                    myPcb = tpcb;
                    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: ACCEPT delayed PCB "); Serial.println(state->identifier););

                    tcp_backlog_accepted(tpcb);
                    activeState = state->stateActiveID;
                    return ERR_OK;
                }
            }
        }
    }

    if (tpcb != nullptr) {
        if (state != nullptr) {
            if (millis() - state->connectTimestamp > state->sConnectionTimeout) {

                if ((tpcb->state == ESTABLISHED || tpcb->state == SYN_SENT || tpcb->state == SYN_RCVD)) {

                    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Close off delayed PCB function 1, ID: "); Serial.println(state->identifier););

                    if (state->backlogWasAccepted == false) {
                        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Server: With backlog accepted"););
                        tcp_backlog_accepted(tpcb);
                        state->backlogWasAccepted = true;
                        accepts--;
                    }

                    state->result = tcp_close(tpcb);
                    state->connected = false;
                    if (state->result == ERR_OK) {
                        state->closeTimer = millis();
                    }
                    else {
                        tcp_abort(tpcb);
                        tpcb = nullptr;
                        return ERR_ABRT;
                    }

                    return state->result;
                }
            }
        }
    }

    return ERR_OK;
}

/**************************************************************************************************/

err_t RF24Client::accept(void* arg, struct tcp_pcb* tpcb, err_t err)
{
    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Accept cb, ID: "); Serial.println(simpleCounter + 1););

    if (tpcb == nullptr) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Accepted conn, but no tpcb from: "); Serial.println(ip4addr_ntoa(ip_2_ip4(&tpcb->remote_ip))););
        return ERR_CLSD;
    }

    if (tpcb != nullptr) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Client connect from: "); Serial.println(ip4addr_ntoa(ip_2_ip4(&tpcb->remote_ip))););
    }
    bool actState = activeState;

    if (myPcb != nullptr) {

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Accept w/already connected: Accepted_Conns - Delayed_Conns == "); Serial.println(accepts););
        tcp_backlog_delayed(tpcb);
        accepts++;
        tcp_poll(tpcb, closed_port, 5);
        actState = !activeState;
        gState[actState]->connected = false;
        gState[actState]->backlogWasAccepted = false;
    }
    else {
        myPcb = tpcb;
        tcp_poll(tpcb, serverTimeouts, 8);
        activeState = !activeState;
        actState = activeState;
        gState[actState]->connected = true;
        gState[actState]->backlogWasAccepted = true;
    }

    dataSize[actState] = 0;

    simpleCounter += 1;
    gState[actState]->stateActiveID = actState;
    gState[actState]->identifier = simpleCounter;
    gState[actState]->sConnectionTimeout = serverConnectionTimeout;
    gState[actState]->connectTimestamp = millis();
    gState[actState]->serverTimer = millis();

    tcp_arg(tpcb, RF24Client::gState[actState]);
    tcp_recv(tpcb, srecv_callback);
    tcp_sent(tpcb, sent_callback);
    tcp_err(tpcb, error_callback);

    return ERR_OK;
}

/***************************************************************************************************/
err_t RF24Client::closeConn(void* arg, struct tcp_pcb* tpcb)
{
    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Client: Immediate close"););
    if (tpcb != nullptr) {
        tcp_close(tpcb);
    }

    return ERR_OK;
}

/***************************************************************************************************/

// Callback triggered by lwIP when handshake completes

err_t RF24Client::on_connected(void* arg, struct tcp_pcb* tpcb, err_t err)
{
    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Client: Conn cb"););

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr) {
        /*if (state->cConnectionTimeout > 0) {
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
            if(Ethernet.useCoreLocking){if(Ethernet.useCoreLocking){ ETHERNET_APPLY_LOCK(); } }
    #endif
            tcp_poll(tpcb, clientTimeouts, 30);
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
            if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
        }*/

        state->cConnectionTimeout = clientConnectionTimeout;
        state->clientTimer = millis();
        state->result = err;
        if (err == ERR_OK) {
            state->connected = true;
        }
        else {
            state->connected = false;
        }
    }
    return err;
}
/** \endcond */
#endif // USE_LWIP == 1

/***************************************************************************************************/

#if USE_LWIP < 1
RF24Client::RF24Client() : data(NULL)
{
}
#elif USE_LWIP == 1
RF24Client::RF24Client() : data(0)
{
}
#elif USE_LWIP == 2
RF24Client::RF24Client() : data(0), _lastError(0)
{
    g_rf24client_instance = this;
}
#endif
/*************************************************************/

#if USE_LWIP < 1
RF24Client::RF24Client(uip_userdata_t* conn_data) : data(conn_data)
{
}
#else
/** \cond */
RF24Client::RF24Client(uint32_t data) : data(0)
{
}
/** \endcond */
#endif
/*************************************************************/

uint8_t RF24Client::connected()
{
#if USE_LWIP < 1
    return (data && (data->packets_in != 0 || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
#elif USE_LWIP == 1
    if (gState[activeState] != nullptr) {
        return gState[activeState]->connected;
    }
    return 0;
#elif USE_LWIP == 2

    if (_socket < 0)
        return 0;

    struct zsock_pollfd pfd
    {
    };
    pfd.fd = _socket;
    pfd.events = ZSOCK_POLLIN | ZSOCK_POLLOUT; // Also check if writable
    int rc = zsock_poll(&pfd, 1, 0);
    if (rc < 0)
        return 1;

    // Just check the poll flags, don't peek
    if (pfd.revents & (ZSOCK_POLLHUP | ZSOCK_POLLERR | ZSOCK_POLLNVAL)) {
        return 0; // Closed
    }

    return 1;
#endif
}

/*************************************************************/

int RF24Client::connect(IPAddress ip, uint16_t port)
{

#if USE_LWIP < 1
    #if UIP_ACTIVE_OPEN > 0

    // do{

    stop();
    uip_ipaddr_t ipaddr;
    uip_ip_addr(ipaddr, ip);

    struct uip_conn* conn = uip_connect(&ipaddr, htons(port));

    if (conn)
    {
        #if UIP_CONNECTION_TIMEOUT > 0
        uint32_t timeout = millis();
        #endif

        while ((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED)
        {
            Ethernet.update();

            if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED)
            {
                data = (uip_userdata_t*)conn->appstate;
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(millis()); Serial.print(F(" connected, state: ")); Serial.print(data->state); Serial.print(F(", first packet in: ")); Serial.println(data->packets_in););
                return 1;
            }

        #if UIP_CONNECTION_TIMEOUT > 0
            if ((millis() - timeout) > UIP_CONNECTION_TIMEOUT)
            {
                conn->tcpstateflags = UIP_CLOSED;
                break;
            }
        #endif
        }
    }
        // delay(25);
        // }while(millis()-timer < 175);

    #endif // Active open enabled
#elif USE_LWIP == 1

    if (myPcb != nullptr) {
        _stop();
        return ERR_CLSD;
    }

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if (Ethernet.useCoreLocking) {
        ETHERNET_APPLY_LOCK();
    }
    #endif

    if (myPcb == nullptr) {
        myPcb = tcp_new();
    }

    if (!myPcb) {
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_REMOVE_LOCK();
        }
    #endif
        return 0;
    }

    dataSize[activeState] = 0;
    memset(incomingData[activeState], 0, INCOMING_DATA_SIZE);

    gState[activeState]->connected = false;
    gState[activeState]->result = 0;
    tcp_arg(myPcb, gState[activeState]);
    tcp_err(myPcb, error_callback);
    tcp_recv(myPcb, recv_callback);
    tcp_sent(myPcb, sent_callback);
    //tcp_poll(myPcb, clientTimeouts, 30);

    err_t err = ERR_OK;

    ip_addr_t myIp;
    IP_ADDR4(&myIp, ip[0], ip[1], ip[2], ip[3]);

    err = tcp_connect(myPcb, &myIp, port, on_connected);

    if (err != ERR_OK || gState[activeState]->result != ERR_OK) {
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_REMOVE_LOCK();
        }
    #endif

        stop();
        return ERR_CLSD;
    }

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if (Ethernet.useCoreLocking) {
        ETHERNET_REMOVE_LOCK();
    }
    #endif

    const uint32_t timeoutStart = millis();
    // Simulate blocking by looping until the callback sets 'connected'
    while (!gState[activeState]->connected && millis() - timeoutStart < 5000) {
        Ethernet.update();
    }

    if (clientConnectionTimeout > 0) {
        gState[activeState]->clientPollingSetup = 1;
    }

    return gState[activeState]->connected;

#elif USE_LWIP == 2

    if (port == 0)
        return -EINVAL;

    int sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        return -errno;

    // --- CRITICAL FIX 1: Set the socket to non-blocking BEFORE connecting ---
    int flags = zsock_fcntl(sock, F_GETFL, 0);
    if (flags < 0) {
        int e = errno;
        zsock_close(sock);
        return -e;
    }
    zsock_fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    char ipstr[16];
    snprintf(ipstr, sizeof(ipstr), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    if (net_addr_pton(AF_INET, ipstr, &addr.sin_addr) < 0) {
        zsock_close(sock);
        return -EINVAL;
    }

    int rc = zsock_connect(sock, (const struct sockaddr*)&addr, sizeof(addr));
    if (rc == 0) {

        _socket = sock; // <-- save it here
        _lastError = 0;
        IF_RF24ETHERNET_DEBUG_CLIENT(printk("CONNECT OK fd=%d\n", _socket));
        RF24Server::connectionActive = true;
        return 1;
    }
    if (errno != EINPROGRESS) {
        int e = errno;
        zsock_close(sock);
        return -e;
    }

    // --- CRITICAL FIX 2: Use Zephyr's native uptime timer instead of Arduino's millis() ---
    int64_t start_time = k_uptime_get();

    // Loop for up to 5000 milliseconds (5 seconds)
    while (k_uptime_get() - start_time < 5000) {
        // 1) Service RF24 every iteration
        RF24Ethernet.update();

        // 2) Non-blocking check of connect completion
        struct zsock_pollfd pfd = {
            .fd = sock,
            .events = ZSOCK_POLLOUT,
            .revents = 0,
        };

        rc = zsock_poll(&pfd, 1, 0); // zero timeout
        if (rc < 0) {
            int e = errno;
            zsock_close(sock);
            return -e;
        }

        if (rc > 0 && (pfd.revents & (ZSOCK_POLLOUT | ZSOCK_POLLERR | ZSOCK_POLLHUP))) {
            int soerr = 0;
            socklen_t slen = sizeof(soerr);
            if (zsock_getsockopt(sock, SOL_SOCKET, SO_ERROR, &soerr, &slen) < 0) {
                int e = errno;
                zsock_close(sock);
                return -e;
            }

            if (soerr == 0) {
                // Remove O_NONBLOCK flag to cleanly destroy blocking context
                zsock_fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
                _socket = sock; // <-- save it here
                _lastError = 0;
                IF_RF24ETHERNET_DEBUG_CLIENT(printk("CONNECT OK fd=%d\n", _socket));
                RF24Server::connectionActive = true;
                return 1;
            }

            zsock_fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
            zsock_close(sock);
            return -soerr;
        }

        // --- CRITICAL FIX 3: Use Zephyr's native kernel sleep to yield CPU time ---
        // This yields execution to Zephyr's network workqueues, timers, and scheduler threads
        k_msleep(2);
    }

    // Timed out cleanup
    zsock_fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
    zsock_close(sock);
    return -ETIMEDOUT;

    return 0;

#endif
    return 0;
}

/*************************************************************/

#if USE_LWIP == 1
void dnsCallback(const char* name, const ip_addr_t* ipaddr, void* callback_arg)
{
}
#endif
/*************************************************************/

int RF24Client::connect(const char* host, uint16_t port)
{
    // Look up the host first
    int ret = 0;

#if UIP_UDP
    DNSClient dns;
    IPAddress remote_addr;

    dns.begin(RF24EthernetClass::_dnsServerAddress);
    ret = dns.getHostByName(host, remote_addr);

    if (ret == 1)
    {
    #if defined(ETH_DEBUG_L1) || defined(RF24ETHERNET_DEBUG_DNS)
        Serial.println(F("*UIP Got DNS*"));
    #endif
        return connect(remote_addr, port);
    }
#elif RF24ETHERNET_USE_UDP

    DNSClient dns;
    IPAddress remote_addr;

    dns.begin(RF24EthernetClass::_dnsServerAddress);
    ret = dns.getHostByName(host, remote_addr);

    if (ret == 1)
    {
    #if defined(ETH_DEBUG_L1) || defined(RF24ETHERNET_DEBUG_DNS)
        Serial.println(F("*lwIP Got DNS*"));
    #endif
        return connect(remote_addr, port);
    }

#else  // ! UIP_UDP
    // Do something with the input parameters to prevent compile time warnings
    if (host) {
    };
    if (port) {
    };
#endif // ! UIP_UDP

#if defined(ETH_DEBUG_L1) || defined(RF24ETHERNET_DEBUG_DNS)
    Serial.println(F("* DNS fail*"));
#endif

    return 0;
}

/*************************************************************/

void RF24Client::stop()
{
#if USE_LWIP < 1
    if (data && data->state)
    {

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(millis()); Serial.println(F(" before stop(), with data")););

        data->packets_in = 0;
        data->dataCnt = 0;

        if (data->state & UIP_CLIENT_REMOTECLOSED)
        {
            data->state = 0;
        }
        else
        {
            data->state |= UIP_CLIENT_CLOSE;
        }

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("after stop()")););
    }
    else
    {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(millis()); Serial.println(F(" stop(), data: NULL")););
    }

    data = NULL;
    RF24Ethernet.update();
#elif USE_LWIP == 1

    _stop();

#elif USE_LWIP == 2
    if (_socket >= 0) {
        zsock_close(_socket);
        _socket = -1;
    }
    RF24Server::connectionActive = false;

#endif
}

/***************************************************************************************************/
#if USE_LWIP == 1
void RF24Client::_stop()
{
    tcp_pcb* pcb = myPcb;
    myPcb = nullptr;

    if (pcb != nullptr) {
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_APPLY_LOCK();
        }
    #endif
        if (pcb->state != CLOSED) {
            tcp_arg(pcb, NULL);
            tcp_recv(pcb, NULL);
            tcp_sent(pcb, NULL);
            tcp_err(pcb, NULL);

            err_t err = tcp_close(pcb);
            if (err != ERR_OK) {
                tcp_abort(pcb);
            }
        }
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if (Ethernet.useCoreLocking) {
            ETHERNET_REMOVE_LOCK();
        }
    #endif
    }

    gState[activeState]->connected = false;
    gState[activeState]->dataSentSize = 0;
}
#endif
/*************************************************************/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs)
{
#if USE_LWIP < 1
    return data && rhs.data && (data == rhs.data);
#elif USE_LWIP == 1
    return dataSize[activeState] > 0 ? true : false;
#elif USE_LWIP == 2
    return available();

#endif
}

/*************************************************************/

RF24Client::operator bool()
{
    Ethernet.update();
#if USE_LWIP < 1
    return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in != 0);
#elif USE_LWIP == 1
    return dataSize[activeState] > 0 ? true : false;
#elif USE_LWIP == 2

    return available();
#endif
}

/*************************************************************/

size_t RF24Client::write(uint8_t c)
{
    return _write(data, &c, 1);
}

/*************************************************************/

size_t RF24Client::write(const uint8_t* buf, size_t size)
{
    return _write(data, buf, size);
}

/*************************************************************/
#if USE_LWIP < 1
size_t RF24Client::_write(uip_userdata_t* u, const uint8_t* buf, size_t size)
#else
size_t RF24Client::_write(uint8_t* data, const uint8_t* buf, size_t size)

#endif

{

#if USE_LWIP < 1
    size_t total_written = 0;
    size_t payloadSize = rf24_min(size, UIP_TCP_MSS);
    uint32_t start = millis();

test2:

    Ethernet.update();

    if (millis() - start > 5000)
    {
        if (u) {
            u->hold = false;
        }
        return total_written;
    }

    if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)) && (u->state & UIP_CLIENT_CONNECTED))
    {
        if (u->out_pos + payloadSize > UIP_TCP_MSS || u->hold)
        {
            goto test2;
        }

        IF_RF24ETHERNET_DEBUG_CLIENT(
            Serial.println();
            Serial.print(millis());
            Serial.print(F(" UIPClient.write: writePacket("));
            Serial.print(u->packets_out);
            Serial.print(F(") pos: "));
            Serial.print(u->out_pos);
            Serial.print(F(", buf["));
            Serial.print(size - total_written);
            Serial.print(F("]: '"));
            Serial.write((uint8_t*)buf + total_written, payloadSize);
            Serial.println(F("'")););

        memcpy(u->myData + u->out_pos, buf + total_written, payloadSize);
        u->packets_out = 1;
        u->out_pos += payloadSize;
        total_written += payloadSize;

        if (total_written < size)
        {
            size_t remain = size - total_written;
            payloadSize = rf24_min(remain, UIP_TCP_MSS);
            goto test2;
        }

        u->hold = false;
        return u->out_pos;
    }

    if (u) {
        u->hold = false;
    }
    return 0;
#elif USE_LWIP == 1

    bool initialActiveState = activeState;
    size_t chunk = MAX_PAYLOAD_SIZE - 14; // 14 = Ethernet/link-layer header bytes reserved per frame
    size_t position = 0;

    uint32_t timeout = millis();
    while (size > chunk) {
        if (millis() - timeout > 3000) {
            break;
        }
        if (myPcb == nullptr)
            return 0;

        gState[initialActiveState]->dataSentSize += chunk;
        err_t write_err = blocking_write(myPcb, gState[initialActiveState], reinterpret_cast<const char*>(&buf[position]), chunk);

        if (write_err != ERR_OK) {
            gState[initialActiveState]->result = write_err;
            gState[initialActiveState]->connected = false;
            _stop();
            return 0;
        }
        position += chunk;
        size -= chunk;
        Ethernet.update();
    }

    if (myPcb == nullptr)
        return 0;

    gState[initialActiveState]->dataSentSize += chunk;
    err_t write_err = blocking_write(myPcb, gState[initialActiveState], reinterpret_cast<const char*>(&buf[position]), size);

    if (write_err != ERR_OK) {
        gState[initialActiveState]->result = write_err;
        gState[initialActiveState]->connected = false;
        _stop();
        return 0;
    }

    return position + size;
#elif USE_LWIP == 2

    RF24Client* self = RF24Client::g_rf24client_instance;
    if (!self || !buf || size == 0)
        return 0;

    if (self->_socket < 0) {
        self->_lastError = ENOTCONN;
        return 0;
    }

    size_t total = 0;
    while (total < size) {

        ssize_t n = zsock_send(self->_socket, buf + total, size - total, 0);

        if (n > 0) {
            total += (size_t)n;
            continue;
        }
        if (n == 0)
            break;

        int err = errno;
        self->_lastError = err;
        if (err == EINTR)
            continue;
        if (err == EAGAIN || err == EWOULDBLOCK)
            break;
        break;
    }
    Ethernet.update();
    return total;

#endif
}

/*************************************************************/

void uip_log(char* msg)
{
    // Serial.println();
    // Serial.println("** UIP LOG **");
    // Serial.println(msg);
    if (msg)
    {
    };
}

/*************************************************************/
#if USE_LWIP < 1
void serialip_appcall(void)
{
    uip_userdata_t* u = (uip_userdata_t*)uip_conn->appstate;

    /*******Connected**********/
    if (!u && uip_connected())
    {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_connected")););

        u = (uip_userdata_t*)EthernetClient::_allocateData();

        if (u)
        {
            uip_conn->appstate = u;
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(F("UIPClient allocated state: ")); Serial.println(u->state, BIN););
        }
        else
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient allocation failed")););
        }
    }

    #if UIP_CONNECTION_TIMEOUT > 0
    if (u && u->connectTimeout > 0) {
        if (millis() - u->connectTimer > u->connectTimeout) {
            u->state |= UIP_CLIENT_CLOSE;
            u->connectTimer = millis();
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println("UIP Client close(timeout)"););
        }
    }
    #endif

    /*******User Data RX**********/
    if (u)
    {
        if (uip_newdata())
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient uip_newdata, uip_len:")); Serial.println(uip_len););
    #if UIP_CONNECTION_TIMEOUT > 0
            u->connectTimer = millis();
    #endif
            u->hold = (u->out_pos = (u->windowOpened = (u->packets_out = false)));

            if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                uip_stop();
                u->state &= ~UIP_CLIENT_RESTART;
                u->windowOpened = false;
                u->restartTime = millis();

                uint16_t writePos = u->in_pos + u->dataCnt;
                uint16_t incomingLen = uip_datalen();

                if (writePos <= OUTPUT_BUFFER_SIZE && incomingLen <= (OUTPUT_BUFFER_SIZE - writePos))
                {
                    memcpy(&u->myData[writePos], uip_appdata, incomingLen);
                    u->dataCnt += incomingLen;
                    u->packets_in = 1;
                }
                else
                {
                    IF_RF24ETHERNET_DEBUG_CLIENT(
                        Serial.println(F("UIPClient RX overflow, closing connection")););
                    u->state |= UIP_CLIENT_CLOSE;
                }
            }
            goto finish;
        }

        /*******Closed/Timed-out/Aborted**********/
        // If the connection has been closed, save received but unread data.
        if (uip_closed() || uip_timedout() || uip_aborted())
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_closed")););
            // drop outgoing packets not sent yet:
            u->packets_out = 0;

            if (u->packets_in)
            {
                ((uip_userdata_closed_t*)u)->lport = uip_conn->lport;
                u->state |= UIP_CLIENT_REMOTECLOSED;
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient close 1")););
            }
            else
            {
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient close 2")););
                u->state = 0;
            }

            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("after UIPClient uip_closed")););
            uip_conn->appstate = NULL;
            goto finish;
        }

        /*******ACKED**********/
        if (uip_acked())
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_acked")););
            u->state &= ~UIP_CLIENT_RESTART;
            u->hold = (u->out_pos = (u->windowOpened = (u->packets_out = false)));
            u->restartTime = millis();
    #if UIP_CONNECTION_TIMEOUT > 0
            u->connectTimer = millis();
    #endif
        }

        /*******Polling**********/
        if (uip_poll() || uip_rexmit())
        {
            if (uip_rexmit()) {
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(F("ReXmit, Len: ")););
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(u->out_pos));
                uip_len = u->out_pos;
                uip_send(u->myData, u->out_pos);
                u->hold = true;
                goto finish;
            }
            // IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.println(F("UIPClient uip_poll")); );

            if (u->packets_out != 0 && !u->hold)
            {
                uip_len = u->out_pos;
                uip_send(u->myData, u->out_pos);
                u->hold = true;
                goto finish;
            }

            // Restart mechanism to keep connections going
            // Only call this if the TCP window has already been re-opened, the connection is being polled, but no data
            // has been acked
            if (!(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {

                if (u->windowOpened == true && u->state & UIP_CLIENT_RESTART && millis() - u->restartTime > u->restartInterval)
                {
                    u->restartTime = millis();
    #if defined RF24ETHERNET_DEBUG_CLIENT || defined ETH_DEBUG_L1
                    Serial.println();
                    Serial.print(millis());
        #if UIP_CONNECTION_TIMEOUT > 0
                    Serial.print(F(" UIPClient Re-Open TCP Window, time remaining before abort: "));
                    Serial.println(UIP_CONNECTION_TIMEOUT - (millis() - u->connectTimer));
        #endif
    #endif
                    u->restartInterval += 500;
                    u->restartInterval = rf24_min(u->restartInterval, 7000);
                    uip_restart();
                }
            }
        }

        /*******Close**********/
        if (u->state & UIP_CLIENT_CLOSE)
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient state UIP_CLIENT_CLOSE")););

            if (u->packets_out == 0)
            {
                u->state = 0;
                uip_conn->appstate = NULL;
                uip_close();
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("no blocks out -> free userdata")););
            }
            else
            {
                uip_stop();
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("blocks outstanding transfer -> uip_stop()")););
            }
        }
finish:;

        if (u->state & UIP_CLIENT_RESTART && !u->windowOpened)
        {
            if (!(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                uip_restart();
    #if defined ETH_DEBUG_L1
                Serial.println();
                Serial.print(millis());
                Serial.println(F(" UIPClient Re-Open TCP Window"));
    #endif
                u->windowOpened = true;
                u->restartInterval = UIP_WINDOW_REOPEN_DELAY; //.75 seconds
                u->restartTime = millis();
            }
        }
    }
}
#endif
/*******************************************************/
#if USE_LWIP < 1
uip_userdata_t* RF24Client::_allocateData()
{
    for (uint8_t sock = 0; sock < UIP_CONNS; sock++)
    {
        uip_userdata_t* data = &RF24Client::all_data[sock];
        if (!data->state)
        {
            data->state = sock | UIP_CLIENT_CONNECTED;
            data->packets_in = 0;
            data->packets_out = 0;
            data->dataCnt = 0;
            data->in_pos = 0;
            data->out_pos = 0;
            data->hold = 0;
            data->restartTime = millis();
            data->restartInterval = 5000;
    #if (UIP_CONNECTION_TIMEOUT > 0)
            data->connectTimer = millis();
            data->connectTimeout = UIP_CONNECTION_TIMEOUT;
    #endif
            return data;
        }
    }
    return NULL;
}
#endif

int RF24Client::waitAvailable(uint32_t timeout)
{
    uint32_t start = millis();
    while (available() < 1)
    {
        if (millis() - start > timeout)
        {
            return 0;
        }
        RF24Ethernet.update();
    }
    return available();
}

/*************************************************************/

int RF24Client::available()
{
    RF24Ethernet.update();
#if USE_LWIP < 1
    if (*this)
    {
        return _available(data);
    }
#else
    return _available(data);
#endif
    return 0;
}

/*************************************************************/
#if USE_LWIP < 1
int RF24Client::_available(uip_userdata_t* u)
#else
int RF24Client::_available(uint8_t* data)
#endif
{
#if USE_LWIP < 1
    if (u->packets_in)
    {
        return u->dataCnt;
    }
#elif USE_LWIP == 1
    return dataSize[activeState];
#elif USE_LWIP == 2

    RF24Client* self = RF24Client::g_rf24client_instance;
    if (!self || self->_socket < 0)
        return 0;

    // First: is there readable/hup state?
    struct zsock_pollfd pfd {};
    pfd.fd = self->_socket;
    pfd.events = ZSOCK_POLLIN;
    int pr = zsock_poll(&pfd, 1, 0);
    if (pr <= 0)
        return 0;

    // If peer closed, report 0 available
    //if (pfd.revents & (ZSOCK_POLLHUP | ZSOCK_POLLNVAL)) return 0;
    //if (!(pfd.revents & ZSOCK_POLLIN)) return 0;

    // Peek queued bytes without consuming
    int n = zsock_recv(self->_socket, peekBuffer, sizeof(peekBuffer),
                       ZSOCK_MSG_PEEK | ZSOCK_MSG_DONTWAIT);

    if (n > 0)
        return n; // bytes currently queued (up to 1024)
    if (n == 0)
        return 0; // closed cleanly
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
        return 0;
    return 0;
#endif
    return 0;
}

/*************************************************************/

int RF24Client::read(uint8_t* buf, size_t size)
{
#if USE_LWIP < 1
    if (*this)
    {
        if (!data->packets_in)
        {
            return -1;
        }
        if (data->in_pos > OUTPUT_BUFFER_SIZE || data->dataCnt > OUTPUT_BUFFER_SIZE || (data->in_pos + data->dataCnt) > OUTPUT_BUFFER_SIZE)
        {
            data->state |= UIP_CLIENT_CLOSE;
            data->in_pos = 0;
            data->dataCnt = 0;
            return -1;
        }
        size = rf24_min(data->dataCnt, size);
        memcpy(buf, &data->myData[data->in_pos], size);
        data->dataCnt -= size;

        data->in_pos += size;

        if (!data->dataCnt)
        {
            data->packets_in = 0;
            data->in_pos = 0;

            if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) && !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                data->state |= UIP_CLIENT_RESTART;
                data->restartTime = 0;

                IF_ETH_DEBUG_L2(Serial.print(F("UIPClient set restart ")); Serial.println(data->state & UIP_CLIENT_SOCKETS); Serial.println(F("**")); Serial.println(data->state, BIN); Serial.println(F("**")); Serial.println(UIP_CLIENT_SOCKETS, BIN); Serial.println(F("**")););
            }
            else
            {
                IF_ETH_DEBUG_L2(Serial.print(F("UIPClient stop?????? ")); Serial.println(data->state & UIP_CLIENT_SOCKETS); Serial.println(F("**")); Serial.println(data->state, BIN); Serial.println(F("**")); Serial.println(UIP_CLIENT_SOCKETS, BIN); Serial.println(F("**")););
            }

            if (data->packets_in == 0)
            {
                if (data->state & UIP_CLIENT_REMOTECLOSED)
                {
                    data->state = 0;
                    data = NULL;
                }
            }
        }
        return size;
    }

    return -1;
#elif USE_LWIP == 1

    if (available()) {

        if (size >= dataSize[activeState]) {
            memcpy(&buf[0], &incomingData[activeState][0], dataSize[activeState]);
            memmove(&incomingData[activeState][0], &incomingData[activeState][dataSize[activeState]], dataSize[activeState]);
            size = dataSize[activeState];
            dataSize[activeState] = 0;
            return size;
        }
        else {
            memcpy(&buf[0], &incomingData[activeState][0], size);
            memmove(&incomingData[activeState][0], &incomingData[activeState][size], dataSize[activeState] - size);
            dataSize[activeState] -= size;
            return size;
        }
    }
    return -1;
#elif USE_LWIP == 2

    if (!buf || size == 0)
        return 0;
    if (_socket < 0) {
        _lastError = ENOTCONN;
        return -1;
    }

    int n = zsock_recv(_socket, buf, size, ZSOCK_MSG_DONTWAIT);

    if (n > 0) {
        return n; // got bytes
    }

    if (n == 0) {
        // Peer performed orderly shutdown.
        // Do NOT close fd here; let caller decide via connected()/stop().
        RF24Server::connectionActive = false;
        _lastError = 0;
        return 0;
    }

    // n < 0
    int err = errno;
    _lastError = err;

    if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR) {
        // no data yet, try again later
        return 0;
    }

    // real error; optionally keep socket open unless clearly unusable
    if (err == EBADF || err == ENOTSOCK) {
        _socket = -1; // already invalid
    }

    return -1;
#endif
}

/*************************************************************/

int RF24Client::read()
{
    uint8_t c;
    if (read(&c, 1) < 0)
        return -1;
    return c;
}

/*************************************************************/

int RF24Client::peek()
{
    if (available())
    {
#if USE_LWIP < 1
        return data->myData[data->in_pos];
#elif USE_LWIP == 1
        return incomingData[activeState][0];
#elif USE_LWIP == 2
        return 0;
#endif
    }
    return -1;
}

/*************************************************************/

void RF24Client::flush()
{
#if USE_LWIP < 1
    if (*this)
    {
    #if USE_LWIP < 1
        data->packets_in = 0;
        data->dataCnt = 0;
    #else
        data = 0;
    #endif
    }
#elif USE_LWIP == 1
    dataSize[activeState] = 0;
    gState[activeState]->dataSentSize = 0;
#elif USE_LWIP == 2

#endif
}
