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

#if USE_LWIP < 1

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN + UIP_IPTCPH_LEN
uip_userdata_t RF24Client::all_data[UIP_CONNS];

#else
   // #define LWIP_ERR_T uint32_t

    //
    #if !defined ETHERNET_USING_LWIP_ARDUINO
        #include "lwip\tcp.h"
        #include "lwip/tcpip.h"
        #include "lwip/timeouts.h"
    #else
        #include "lwip\include\lwip\tcp.h"
        #include "lwip\include\lwip\tcpip.h"
    #endif

    #include "RF24Ethernet.h"

RF24Client::ConnectState* RF24Client::gState[2];
char* RF24Client::incomingData[2];
uint16_t RF24Client::dataSize[2];
struct tcp_pcb* RF24Client::myPcb;
uint32_t RF24Client::clientConnectionTimeout;
uint32_t RF24Client::serverConnectionTimeout;
uint32_t RF24Client::simpleCounter;
bool RF24Client::activeState;

/***************************************************************************************************/

// Called when the remote host acknowledges receipt of data
err_t RF24Client::sent_callback(void* arg, struct tcp_pcb* tpcb, u16_t len)
{

    ConnectState* state = (ConnectState*)arg;
    if (state != nullptr) {
        state->serverTimer = millis();
        state->clientTimer = millis();
        IF_ETH_DEBUG_L1( Serial.println("Client: Sent cb"); );
    

        state->waiting_for_ack = false; // Data is successfully out
        state->finished = true;
    }

    return ERR_OK;
}

/***************************************************************************************************/

err_t RF24Client::blocking_write(struct tcp_pcb* fpcb, ConnectState* fstate, const char* data, size_t len)
{

    if(fpcb == nullptr){
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Client: Tx with no fpcb"); );
        return ERR_CLSD;
    }

    if(!fstate->connected){
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Client: Tx with no connection"); );
        return ERR_CLSD;
    }

    uint32_t timeout = millis() + serverConnectionTimeout;
    while (len > tcp_sndbuf(fpcb)) {
        Ethernet.update();
        if (millis() > timeout) {
            IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Client: TCP Send Buffer full"); );
            return ERR_BUF;
        }
    }    
 
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
   if(Ethernet.useCoreLocking){ ETHERNET_APPLY_LOCK();  }
    #endif
   
    err_t err = ERR_CLSD;
    if(fpcb != nullptr){
       err = tcp_write(fpcb, data, len, TCP_WRITE_FLAG_COPY);
    }
    
    //Ethernet.update();
    if (err != ERR_OK) {
        if(fstate != nullptr){
            fstate->waiting_for_ack = false;
            fstate->finished = true;
        }
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Client: BLK Write fail 2: "); Serial.println((int)err); );

#if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(Ethernet.useCoreLocking ){ ETHERNET_REMOVE_LOCK();} 
    #endif
        return err;
    }

    if (fpcb != nullptr && fpcb->state != CLOSED && fstate->connected) {
        tcp_sent(fpcb, sent_callback);
    }
    else {
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Client: TCP OUT FAIL"); );

#if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK();  }
    #endif
        return ERR_BUF;
    }
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(Ethernet.useCoreLocking ){ ETHERNET_REMOVE_LOCK();}
    #endif

    volatile uint32_t timer = millis() + 5000;
    while (fstate != nullptr && fstate->waiting_for_ack && !fstate->finished) {
        if (millis() > timer) {
            if(fstate != nullptr){
                fstate->finished = true;
                fstate->result = -1;
            }
            break;
        }
        Ethernet.update();
    }

    return ERR_OK;
    /*if(fstate != nullptr){
        return fstate->result;
    }
    return ERR_CLSD;*/
}

/***************************************************************************************************/

void RF24Client::error_callback(void* arg, err_t err)
{

    ConnectState* state = (ConnectState*)arg;
    if (state != nullptr) {
        state->result = err;
        state->connected = false;
        state->finished = true; // Break the blocking loop
        state->waiting_for_ack = false;
    }
    IF_RF24ETHERNET_DEBUG_CLIENT(  Serial.println("Client: Err cb: ");  Serial.println((int)err); );    
}

/***************************************************************************************************/

err_t RF24Client::srecv_callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr) {
        state->serverTimer = millis();
    }

    if (p == nullptr) {
        if(state != nullptr){
          state->connected = false;
          //state->finished = true; // Break the loop
        }
        if(tpcb != nullptr){
            tcp_close(tpcb);
            myPcb = nullptr;
        }
        return ERR_OK;
    }
    if (err != ERR_OK || state == nullptr) {
        if (p)
            pbuf_free(p);
        return ERR_OK;
    }

    const uint8_t* data = static_cast<const uint8_t*>(p->payload);
    
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Copy data to "); Serial.println(state->stateActiveID); );
        if (dataSize[state->stateActiveID] + p->len < INCOMING_DATA_SIZE){
            memcpy(&incomingData[state->stateActiveID][dataSize[state->stateActiveID]], data, p->len);
            dataSize[state->stateActiveID] += p->len;
        }else{
            IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Server: srecv - Out of incoming buffer space"); );
        }

    // Process data
    tcp_recved(tpcb, p->len);

    pbuf_free(p);
    return ERR_OK;
}

/***************************************************************************************************/

err_t RF24Client::recv_callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{
    
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Client: Recv cb"); );

    ConnectState* state = (ConnectState*)arg;
    if (p == nullptr) {
        if(state != nullptr){
            state->connected = false;
            state->finished = true; // Break the loop
        }
        if(tpcb != nullptr){
            tcp_close(tpcb);
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
    const uint8_t* data = static_cast<const uint8_t*>(p->payload);
    if (dataSize[activeState] + p->len < INCOMING_DATA_SIZE) {
        memcpy(&incomingData[activeState][dataSize[activeState]], data, p->len);
        dataSize[activeState] += p->len;
    }
    else {
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Client: recv - Out of incoming buffer space"); );
    }

    if(tpcb != nullptr){
        tcp_recved(tpcb, p->len);
    }
    pbuf_free(p);
        
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
                IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Client: Closed Client PCB TIMEOUT"); );
                err_t err = tcp_close(tpcb);
                state->result = err;
                state->connected = false;
                state->finished = true; // Break the blocking loop
                state->waiting_for_ack = false;
            }
        }
    }
    return ERR_OK;
}

/***************************************************************************************************/
int32_t accepts = 0;

err_t RF24Client::serverTimeouts(void* arg, struct tcp_pcb* tpcb)
{

    ConnectState* state = (ConnectState*)arg;

    if (state != nullptr && tpcb != nullptr) {
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Stimeout cb "); Serial.println(millis() - state->serverTimer); );
        
        if (millis() - state->serverTimer > state->sConnectionTimeout && state->backlogWasClosed == false) {
            //if (tpcb->state == ESTABLISHED || tpcb->state == SYN_SENT || tpcb->state == SYN_RCVD) {
                IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Server: Closed Server PCB TIMEOUT "); );
                tcp_close(tpcb);
                state->closeTimer = millis();
                state->backlogWasClosed = true;
                dataSize[activeState] = 0;
                state->connected = false;
                state->finished = true; 
                if(state->backlogWasAccepted == false ){
                    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Server: With backlog accepted"); );
                    tcp_backlog_accepted(tpcb);
                    accepts--;
                }
                return ERR_OK;
                
           // }
        }
            if(state->backlogWasClosed == true){
                if(millis() - state->closeTimer > 5000){
                    tcp_abort(tpcb);
                    myPcb = nullptr;
                    return ERR_ABRT;
                }
            
            }
    }
    return ERR_OK;
}

/***************************************************************************************************/

err_t RF24Client::closed_port(void* arg, struct tcp_pcb* tpcb)
{

    ConnectState* state = (ConnectState*)arg;

    if(state != nullptr){
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Client Poll Cb ID: "); Serial.println(state->identifier));
    }
    
    if (myPcb == nullptr ) {
        if (state != nullptr && tpcb != nullptr) {

            if ((tpcb->state == ESTABLISHED || tpcb->state == SYN_SENT || tpcb->state == SYN_RCVD)) {
                if(state->backlogWasAccepted == false && state->backlogWasClosed == false){
                    
                    state->backlogWasAccepted = true;
                    state->connectTimestamp = millis();
                    state->connected = true;
                    state->finished = false;
                    accepts--;
                    myPcb = tpcb;
                    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: ACCEPT delayed PCB "); Serial.println(state->identifier); );

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
                  if(state->backlogWasClosed == false){
                      
                    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Close off delayed PCB function 1, ID: "); Serial.println(state->identifier); );
                    if(tcp_close(tpcb) == ERR_OK){
                        state->backlogWasClosed = true;
                        state->closeTimer = millis();
                        state->finished = true;
                        
                    }
                    
                    if(state->backlogWasAccepted == false){
                      IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Server: With backlog accepted"); );
                      tcp_backlog_accepted(tpcb);
                      state->backlogWasAccepted = true;
                      accepts--;
                    }

                    return ERR_OK;
                  }else{
                      IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Killing off TPCB already closed function 1, ID: "); );
      
                      if(state != nullptr){                    
                        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(state->identifier); );
                      }
                      if(millis() - state->closeTimer > 5000){
                          tcp_abort(tpcb);
                        return ERR_ABRT;
                      }
                  }
                }
            }
        }        
    }
	if (tpcb != nullptr) {
       if(state != nullptr){
        if (millis() - state->connectTimestamp > state->sConnectionTimeout) {
            if(state->backlogWasClosed == false){
                IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Close off delayed PCB function 2, ID "); Serial.println(state->identifier); );               
                if(tcp_close(tpcb) == ERR_OK){
                    state->backlogWasClosed = true;
                    state->closeTimer = millis();
                    state->finished = true;
                }
                if(state->backlogWasAccepted == false){
                    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Server: With backlog accepted"); );
                    tcp_backlog_accepted(tpcb);
                    state->backlogWasAccepted = true;
                    accepts--;
                }

                    return ERR_OK;
            }else{
                    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Killing off TPCB already closed function 2, ID: "); );
                    if(state != nullptr){                  
                        Serial.println(state->identifier);
                        if(millis() - state->closeTimer > 5000){
                            tcp_abort(tpcb);
                            return ERR_ABRT;
                        }
                    }
            }
        }
       }
    }
     
    return ERR_OK;
}

/**************************************************************************************************/

err_t RF24Client::accept(void* arg, struct tcp_pcb* tpcb, err_t err)
{
    IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print("Server: Accept cb, ID: "); Serial.println(simpleCounter + 1); );
    ConnectState* state = (ConnectState*)arg;

if(tpcb != nullptr){
    #if !defined ESP32 && !defined ARDUINO_ARCH_RP2040 && !defined ARDUINO_ARCH_RP2350
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Client connect from: "); IPAddress remIP; remIP[0] = ip4_addr_get_byte(&tpcb->remote_ip, 0); remIP[1] = ip4_addr_get_byte(&tpcb->remote_ip, 1); 
    remIP[2] = ip4_addr_get_byte(&tpcb->remote_ip, 2); remIP[3] = ip4_addr_get_byte(&tpcb->remote_ip, 3); Serial.println(remIP); );
    #else
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Client connect from: "); Serial.println(IPAddress((&tpcb->remote_ip))); );
    #endif
}
    bool actState = activeState;
    
    
    if (myPcb != nullptr || gState[activeState]->connected == true) {
		
        IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print("Server: Accept w/already connected: Delayed_Conns - Accepted_Conns == "); Serial.println(accepts); );
        tcp_backlog_delayed(tpcb);
        accepts++;
        tcp_poll(tpcb, closed_port, 5);
        actState = !activeState;
        gState[actState]->connected = false;
    }else{
        myPcb = tpcb;
        tcp_poll(tpcb, serverTimeouts, 8);
        gState[actState]->connected = true;
    }
    
    dataSize[actState] = 0;
    
    simpleCounter+=1;
    gState[actState]->stateActiveID = actState;
    gState[actState]->identifier = simpleCounter;
    gState[actState]->finished=false;
    gState[actState]->sConnectionTimeout = serverConnectionTimeout;
    gState[actState]->waiting_for_ack = false;
    gState[actState]->backlogWasAccepted = false;
    gState[actState]->backlogWasClosed=false;
    gState[actState]->connectTimestamp=millis();
    gState[actState]->serverTimer = millis();
    
    tcp_arg(tpcb, RF24Client::gState[actState]);    
    tcp_recv(tpcb, srecv_callback);
    tcp_sent(tpcb, sent_callback);


    return ERR_OK;
}

/***************************************************************************************************/
err_t RF24Client::closeConn(void* arg, struct tcp_pcb* tpcb)
{ 
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Client: Immediate close"); );
    if (tpcb != nullptr) {
        tcp_close(tpcb);
    }
    
    return ERR_OK;
}

/***************************************************************************************************/

// Callback triggered by lwIP when handshake completes

err_t RF24Client::on_connected(void* arg, struct tcp_pcb* tpcb, err_t err)
{
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println("Client: Conn cb"); );

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
        state->finished = true;
        if(err == ERR_OK){
            state->connected = true;
        }else{
            state->connected = false;
        }
        state->waiting_for_ack = false;
    }
    return err;
}

#endif // USE_LWIP > 1
/***************************************************************************************************/

#if USE_LWIP < 1
RF24Client::RF24Client() : data(NULL) {}
#else
RF24Client::RF24Client() : data(0)
{
    clientConnectionTimeout = 0;
    serverConnectionTimeout = 30000;
}

#endif
/*************************************************************/

#if USE_LWIP < 1
RF24Client::RF24Client(uip_userdata_t* conn_data) : data(conn_data) {}
#else
RF24Client::RF24Client(uint32_t data) : data(0)
{
    clientConnectionTimeout = 0;
    serverConnectionTimeout = 30000;
}
#endif
/*************************************************************/

uint8_t RF24Client::connected()
{
#if USE_LWIP < 1
    return (data && (data->packets_in != 0 || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
#else
    if (gState[activeState] != nullptr) {
        return gState[activeState]->connected;
    }
    return 0;
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
#else

    
    if (myPcb != nullptr) {
        if (myPcb->state == ESTABLISHED || myPcb->state == SYN_SENT || myPcb->state == SYN_RCVD) {
     
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
            if(Ethernet.useCoreLocking){ ETHERNET_APPLY_LOCK(); }
    #endif
            tcp_close(myPcb);
    
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
            if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
            Ethernet.update();
            return false;
        }
    }
     
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
   if(Ethernet.useCoreLocking){ ETHERNET_APPLY_LOCK(); }
    #endif
    myPcb = tcp_new();
    if (!myPcb) {
    
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
        return 0;
    }

    dataSize[activeState] = 0;
    memset(incomingData[activeState], 0, sizeof(incomingData[activeState]));

    if (gState[activeState] == nullptr) {
        gState[activeState] = new ConnectState;
    }
    gState[activeState]->finished = false;
    gState[activeState]->connected = false;
    gState[activeState]->result = 0;
    gState[activeState]->waiting_for_ack = false;


    tcp_arg(myPcb, gState[activeState]);
    tcp_err(myPcb, error_callback);
    tcp_recv(myPcb, recv_callback);
    //tcp_poll(myPcb, clientTimeouts, 30);

    err_t err = ERR_OK;
    ip4_addr_t myIp;
    #if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_RP2040 || defined ARDUINO_ARCH_RP2350
    IP4_ADDR(&myIp, ip[0], ip[1], ip[2], ip[3]);
    ip_addr_t generic_addr;
    ip_addr_copy_from_ip4(generic_addr, myIp);
    err = tcp_connect(myPcb, &generic_addr, port, on_connected);
    #else
    IP4_ADDR(&myIp, ip[0], ip[1], ip[2], ip[3]);
    // Start non-blocking connection
    err = tcp_connect(myPcb, &myIp, port, on_connected);
    #endif


    if (err != ERR_OK) {
        if (myPcb) {
            tcp_close(myPcb);
        }
        gState[activeState]->connected = false;
        gState[activeState]->finished = true;
    
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
        return ERR_CLSD;
    }
    
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
    uint32_t timeout = millis() + 5000;
    // Simulate blocking by looping until the callback sets 'finished'
    while (!gState[activeState]->finished && millis() < timeout) {
        Ethernet.update();
    }
    
    if(clientConnectionTimeout > 0){
      gState[activeState]->clientPollingSetup = 1;
    }
    
    return gState[activeState]->connected;

#endif
    return 0;
}

/*************************************************************/

#if USE_LWIP > 1
void dnsCallback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    
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

    return ret;
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
#else

        if (myPcb != nullptr) {

            if (myPcb->state == ESTABLISHED || myPcb->state == SYN_SENT || myPcb->state == SYN_RCVD) {

     
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
               if(Ethernet.useCoreLocking){ ETHERNET_APPLY_LOCK(); }
    #endif
                tcp_close(myPcb);
    
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
                if(Ethernet.useCoreLocking){ ETHERNET_REMOVE_LOCK();}
    #endif

            }
        }

        if(gState[activeState] != nullptr){
            gState[activeState]->connected = false;
            gState[activeState]->finished = true;
        }

#endif
}

/*************************************************************/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs)
{
#if USE_LWIP < 1
    return data && rhs.data && (data == rhs.data);
#else
    return dataSize[activeState] > 0 ? true : false;
#endif
}

/*************************************************************/

RF24Client::operator bool()
{
    Ethernet.update();
#if USE_LWIP < 1
    return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in != 0);
#else
    return dataSize[activeState] > 0 ? true : false;
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

test2:

    Ethernet.update();
    if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)) && u->state & (UIP_CLIENT_CONNECTED))
    {

        if (u->out_pos + payloadSize > UIP_TCP_MSS || u->hold)
        {
            goto test2;
        }

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient.write: writePacket(")); Serial.print(u->packets_out); Serial.print(F(") pos: ")); Serial.print(u->out_pos); Serial.print(F(", buf[")); Serial.print(size - total_written); Serial.print(F("]: '")); Serial.write((uint8_t*)buf + total_written, payloadSize); Serial.println(F("'")););

        memcpy(u->myData + u->out_pos, buf + total_written, payloadSize);
        u->packets_out = 1;
        u->out_pos += payloadSize;

        total_written += payloadSize;

        if (total_written < size)
        {
            size_t remain = size - total_written;
            payloadSize = rf24_min(remain, UIP_TCP_MSS);

            // RF24EthernetClass::update();
            goto test2;
        }
        u->hold = false;
        return u->out_pos;
    }
    u->hold = false;
    return -1;
#else

    if (myPcb == nullptr) {
        return ERR_CLSD;
    }
    
    bool initialActiveState = activeState;
    
    if( gState[initialActiveState] == nullptr){
        return ERR_CLSD;
    }
    
    char buffer[size];
    uint32_t position = 0;
    uint32_t timeout1 = millis() + 3000;

    while (size > MAX_PAYLOAD_SIZE - 14 && millis() < timeout1) {
        memcpy(buffer, &buf[position], MAX_PAYLOAD_SIZE - 14);

        if (myPcb == nullptr ) {
            return ERR_CLSD;
        }
        gState[initialActiveState]->waiting_for_ack = true;
        err_t write_err = blocking_write(myPcb, gState[initialActiveState], buffer, MAX_PAYLOAD_SIZE - 14);
        
        if (write_err != ERR_OK) {
            return (write_err);
        }        
        position += MAX_PAYLOAD_SIZE - 14;
        size -= MAX_PAYLOAD_SIZE - 14;
        Ethernet.update();
    }
    
    memcpy(buffer, &buf[position], size);

    if (myPcb == nullptr){
        return ERR_CLSD;
    }

    gState[initialActiveState]->waiting_for_ack = true;
    err_t write_err = blocking_write(myPcb, gState[initialActiveState], buffer, size);
    
    if (write_err == ERR_OK) {
        return (size);
    }

    return write_err;
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
                memcpy(&u->myData[u->in_pos + u->dataCnt], uip_appdata, uip_datalen());
                u->dataCnt += uip_datalen();

                u->packets_in = 1;
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
#else
    return dataSize[activeState];
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
#else
    

    if (available()) {
        
        int32_t remainder = dataSize[activeState] - size;
        memcpy(&buf[0], &incomingData[activeState][0], size);
        if (remainder > 0) {
            memmove(&incomingData[activeState][0], &incomingData[activeState][size], dataSize[activeState] - size);
        }
        dataSize[activeState] = rf24_max(0,remainder);
        return size;
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
#else
        return incomingData[activeState][0];
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
#else
    while (available()) {
        read();
    }
#endif
}
