/*
 RF24Ethernet.cpp - Arduino implementation of a uIP wrapper class.
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

#if USE_LWIP > 0
RF24EthernetClass::EthQueue RF24EthernetClass::RXQueue __attribute__((aligned(4)));
netif RF24EthernetClass::myNetif;
bool RF24EthernetClass::useCoreLocking;
uint8_t RF24EthernetClass::networkBuffer[MAX_PAYLOAD_SIZE];
IPAddress RF24EthernetClass::_dnsServerAddress;

/*************************************************************/

void RF24EthernetClass::initRXQueue(EthQueue* RXQueue)
{
    RXQueue->nWrite = 0;
    RXQueue->nRead = 0;
}

/*************************************************************/

//Saves RX ethernet frame to the buffer to be processed in the main loop
void RF24EthernetClass::writeRXQueue(EthQueue* RXQueue, const uint8_t* ethFrame, uint16_t lenEthFrame)
{
    if (lenEthFrame > MAX_FRAME_SIZE)
    {
        lenEthFrame = MAX_FRAME_SIZE;
    }
    memcpy(&RXQueue->data[RXQueue->nWrite], ethFrame, lenEthFrame);
    RXQueue->len[RXQueue->nWrite] = lenEthFrame;
    RXQueue->nWrite++;
    RXQueue->nWrite %= MAX_RX_QUEUE;
}

/*************************************************************/

pbuf* RF24EthernetClass::readRXQueue(EthQueue* RXQueue)
{
    if (RXQueue->nWrite != RXQueue->nRead)
    {
        const int ehtFrmLen = RXQueue->len[RXQueue->nRead];
        pbuf* p = pbuf_alloc(PBUF_RAW, MAX_FRAME_SIZE, PBUF_RAM);
        if (p) {
            memcpy(reinterpret_cast<uint8_t*>(p->payload),
                   &RXQueue->data[RXQueue->nRead][0],
                   ehtFrmLen);
            RXQueue->nRead++;
            RXQueue->nRead %= MAX_RX_QUEUE;
            return p;
        }
        else {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

/*************************************************************/

bool RF24EthernetClass::isUnicast(const uint8_t frame)
{
    return (frame & 0x01) == 0;
}

/*************************************************************/

err_t netif_output(struct netif* netif, struct pbuf* p)
{
    void* context = netif->state;
    uint16_t total_len = 0;
    char buf[Ethernet.MAX_FRAME_SIZE]; /* max packet size including VLAN excluding FCS */

    if (p->tot_len > sizeof(buf))
    {
        MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
        return ERR_IF;
    }
    pbuf_copy_partial(p, buf, p->tot_len, 0);
    LINK_STATS_INC(link.xmit);
    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);

    if (p->tot_len < Ethernet.MIN_FRAME_SIZE) // Pad to minimum ETH size
    {
        total_len = Ethernet.MIN_FRAME_SIZE;
    }
    else
    {
        total_len = p->tot_len;
    }

    if (Ethernet.isUnicast(buf[0]))
    {
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }
    else
    {
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    }

    IPAddress gwIP = Ethernet.gatewayIP();
    int16_t nodeAddress = 0;

    //If not the master node
    if (Ethernet.mesh.mesh_address != 0) {
        if (gwIP[3] != buf[19]) {                                       // If not sending to the gateway
            IPAddress local_ip = Ethernet.localIP();
            if(local_ip[0] == buf[16] && local_ip[1] == buf[17]){       // If we are local within the nRF24 network
                //Request an address lookup from the Master node
                nodeAddress = Ethernet.mesh.getAddress((char)buf[19]);  // Do an address lookup
                if (nodeAddress < 0) {
                    nodeAddress = 0;                                    // If the result is negative, send to master
                }
            }                                                           // If this address is outside the nRF24 network, it will be send to master (00)
        }
    }
    else {
        IPAddress local_ip = Ethernet.localIP();
        if(local_ip[0] == buf[16] && local_ip[1] == buf[17]){          // If within the nRF24 radio network, do a lookup, else send to self (00)
            nodeAddress = Ethernet.mesh.getAddress((char)buf[19]);
            if (nodeAddress < 0) {
                return ERR_OK;
            }
        }
    }

    IF_ETH_DEBUG_L1( Serial.print("Net: Out "); Serial.println(nodeAddress, OCT); );
    
    RF24NetworkHeader headerOut(nodeAddress, EXTERNAL_DATA_TYPE);

    if (total_len && total_len < MAX_PAYLOAD_SIZE) {
        if (!RF24Ethernet.network.write(headerOut, buf, total_len)) {
            return ERR_OK;
        }
    }
    return ERR_OK;
}

/*************************************************************/

err_t tun_netif_output(struct netif* netif, struct pbuf* p, const ip4_addr_t* ipaddr)
{
    /* Since this is a TUN/L3 interface, we skip ARP (etharp_output).
       We simply call the linkoutput function to send the raw IP packet. */
    return netif->linkoutput(netif, p);
}

/*************************************************************/

err_t netif_init(struct netif* myNetif)
{

    myNetif->name[0] = 'e';
    myNetif->name[1] = '0';
    myNetif->linkoutput = netif_output;
    myNetif->output = tun_netif_output;
    myNetif->mtu = MAX_PAYLOAD_SIZE-14; //ETHERNET_MTU;
    myNetif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6 | NETIF_FLAG_LINK_UP;
    myNetif->hostname = "TmrNet";
    MIB2_INIT_NETIF(&Ethernet.myNetif, snmp_ifType_ppp, Ethernet.NetIF_Speed_BPS);
    //SMEMCPY(myNetif->hwaddr, &Ethernet.MacAddr, sizeof(myNetif->hwaddr));
    myNetif->hwaddr_len = 0; //sizeof(netif->hwaddr);
    Ethernet.initRXQueue(&Ethernet.RXQueue);
    netif_set_link_up(myNetif);
    return ERR_OK;
}

#endif

/*************************************************************/
#if !defined NRF52_RADIO_LIBRARY
    #if defined(RF24_TAP)
RF24EthernetClass::RF24EthernetClass(RF24& _radio, RF24Network& _network) : radio(_radio), network(_network) // fn_uip_cb(NULL)
{
}

    #else // Using RF24Mesh
RF24EthernetClass::RF24EthernetClass(RF24& _radio, RF24Network& _network, RF24Mesh& _mesh) : radio(_radio), network(_network), mesh(_mesh) // fn_uip_cb(NULL)
{

}
    #endif

#else
    #if defined(RF24_TAP)
RF24EthernetClass::RF24EthernetClass(nrf_to_nrf& _radio, RF52Network& _network) : radio(_radio), network(_network) // fn_uip_cb(NULL)
{
}

    #else // Using RF24Mesh
RF24EthernetClass::RF24EthernetClass(nrf_to_nrf& _radio, RF52Network& _network, RF52Mesh& _mesh) : radio(_radio), network(_network), mesh(_mesh) // fn_uip_cb(NULL)
{
    
}
    #endif
#endif
/*************************************************************/

void RF24EthernetClass::update()
{
    Ethernet.tick();
}

/*************************************************************/

void RF24EthernetClass::use_device()
{
    // Kept for backwards compatibility only
}

/*******************************************************/

void RF24EthernetClass::setMac(uint16_t address)
{
    if (!network.multicastRelay) { // Radio has not been started yet
        radio.begin();
    }

    const uint8_t mac[6] = {0x52, 0x46, 0x32, 0x34, (uint8_t)address, (uint8_t)(address >> 8)};
    // printf("MAC: %o %d\n", address, mac[0]);

#if defined(RF24_TAP)
    uip_seteth_addr(mac);
    network.multicastRelay = 1;
#else
    if (mac[0] == 1) {
        // Dummy operation to prevent warnings if TAP not defined
    };
#endif
    RF24_Channel = RF24_Channel ? RF24_Channel : 97;
    network.begin(RF24_Channel, address);
}

/*******************************************************/

void RF24EthernetClass::setChannel(uint8_t channel)
{
    RF24_Channel = channel;
    if (network.multicastRelay) { // Radio has not been started yet
        radio.setChannel(RF24_Channel);
    }
}

/*******************************************************/

void RF24EthernetClass::begin(IPAddress ip)
{
    IPAddress dns = {8,8,8,8};
    begin(ip, dns);
}

/*******************************************************/

void RF24EthernetClass::begin(IPAddress ip, IPAddress dns)
{
    IPAddress gateway = ip;
    gateway[3] = 1;
    begin(ip, dns, gateway);
}

/*******************************************************/

void RF24EthernetClass::begin(IPAddress ip, IPAddress dns, IPAddress gateway)
{
    IPAddress subnet(255, 255, 255, 0);
    begin(ip, dns, gateway, subnet);
}

/*******************************************************/

void RF24EthernetClass::begin(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet)
{
    configure(ip, dns, gateway, subnet);

}

/*******************************************************/

void RF24EthernetClass::configure(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet)
{
#if !defined(RF24_TAP) // Using RF24Mesh
    mesh.setNodeID(ip[3]);
#endif

#if USE_LWIP < 1
    uip_buf = (uint8_t*)&network.frag_ptr->message_buffer[0];

    uip_ipaddr_t ipaddr;
    uip_ip_addr(ipaddr, ip);
    uip_sethostaddr(ipaddr);
    uip_ip_addr(ipaddr, gateway);
    uip_setdraddr(ipaddr);
    uip_ip_addr(ipaddr, subnet);
    uip_setnetmask(ipaddr);
    _dnsServerAddress = dns;

    timer_set(&this->periodic_timer, CLOCK_SECOND / UIP_TIMER_DIVISOR);

    #if defined(RF24_TAP)
    timer_set(&this->arp_timer, CLOCK_SECOND * 2);
    #endif

    uip_init();
    #if defined(RF24_TAP)
    uip_arp_init();
    #endif
#else
    
    RF24Client::activeState = 0;
    // Allocate data for a single client
    RF24Client::incomingData[RF24Client::activeState] = (char*)malloc(INCOMING_DATA_SIZE);

        #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING && defined ESP32
            wifi_mode_t mode;
            esp_err_t err = esp_wifi_get_mode(&mode);
            if (err == ESP_OK) {
                useCoreLocking = true;
            }else{
                useCoreLocking = false;
            }
        #elif defined RF24ETHERNET_CORE_REQUIRES_LOCKING
            useCoreLocking = true;
        #endif
        
    ip4_addr_t myIp, myMask, myGateway;
    IP4_ADDR(&myIp, ip[0], ip[1], ip[2], ip[3]);
    IP4_ADDR(&myMask, subnet[0], subnet[1], subnet[2], subnet[3]);
    IP4_ADDR(&myGateway, gateway[0], gateway[1], gateway[2], gateway[3]);
    _dnsServerAddress = dns;
    
    void* context = nullptr;
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(useCoreLocking){ ETHERNET_APPLY_LOCK(); }
    #endif
    netif_add(&Ethernet.myNetif, &myIp, &myMask, &myGateway, context, netif_init, ip_input);
    netif_set_default(&Ethernet.myNetif);
    netif_set_up(&Ethernet.myNetif);
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif

#endif
}

/*******************************************************/

void RF24EthernetClass::set_gateway(IPAddress gwIP)
{
#if USE_LWIP < 1
    uip_ipaddr_t ipaddr;
    uip_ip_addr(ipaddr, gwIP);
    uip_setdraddr(ipaddr);
#else
    ip4_addr_t new_gw;
    IP4_ADDR(&new_gw, gwIP[0], gwIP[1], gwIP[2], gwIP[3]);
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(useCoreLocking){ ETHERNET_APPLY_LOCK(); }
    #endif
    netif_set_gw(&Ethernet.myNetif, &new_gw);
    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
#endif
}

/*******************************************************/

void RF24EthernetClass::listen(uint16_t port)
{
#if USE_LWIP < 1
    uip_listen(HTONS(port));
#else

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(useCoreLocking){ ETHERNET_APPLY_LOCK(); } 
    #endif
    RF24Client::myPcb = tcp_new();
    tcp_err(RF24Client::myPcb, RF24Client::error_callback);


    err_t err = tcp_bind(RF24Client::myPcb, IP_ADDR_ANY, port);
    if (err != ERR_OK) {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println("Server: Unable to bind to port"););
    }

    RF24Client::gState[0]->finished = false;
    RF24Client::gState[0]->connected = false;
    RF24Client::gState[0]->result = 0;
    RF24Client::gState[0]->waiting_for_ack = false;

    RF24Client::myPcb = tcp_listen(RF24Client::myPcb);

    tcp_arg(RF24Client::myPcb, &RF24Client::gState[0]);
    tcp_accept(RF24Client::myPcb, RF24Client::accept);

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    if(useCoreLocking){ ETHERNET_REMOVE_LOCK(); }
    #endif
#endif
}

/*******************************************************/

IPAddress RF24EthernetClass::localIP()
{
#if USE_LWIP < 1
    uip_ipaddr_t a;
    uip_gethostaddr(a);
    return ip_addr_uip(a);
#else
    if (netif_is_up(&myNetif)) {
        // Get the IP address structure
        const ip4_addr_t* ip_addr = netif_ip4_addr(&myNetif);
        return (IPAddress(ip_addr->addr));
    }
    return IPAddress {0, 0, 0, 0};
#endif
}

/*******************************************************/

IPAddress RF24EthernetClass::subnetMask()
{
#if USE_LWIP < 1
    uip_ipaddr_t a;
    uip_getnetmask(a);
    return ip_addr_uip(a);
#else
    if (netif_is_up(&myNetif)) {
        // Get the IP address structure
        const ip4_addr_t* ip_addr = netif_ip4_netmask(&myNetif);
        return (IPAddress(ip_addr->addr));
    }
    return IPAddress {0, 0, 0, 0};
#endif
}

/*******************************************************/

IPAddress RF24EthernetClass::gatewayIP()
{
#if USE_LWIP < 1
    uip_ipaddr_t a;
    uip_getdraddr(a);
    return ip_addr_uip(a);
#else
    if (netif_is_up(&myNetif)) {
        // Get the IP address structure
        const ip4_addr_t* ip_addr = netif_ip4_gw(&myNetif);
        return (IPAddress(ip_addr->addr));
    }
    return IPAddress {0, 0, 0, 0};
#endif
}

/*******************************************************/

IPAddress RF24EthernetClass::dnsServerIP()
{
    return _dnsServerAddress;
}

/*******************************************************/
#if USE_LWIP > 0
//Should be call inside PHY RX Ethernet IRQ
void RF24EthernetClass::EthRX_Handler(const uint8_t* ethFrame, const uint16_t lenEthFrame)
{
    LINK_STATS_INC(link.recv);
    MIB2_STATS_NETIF_ADD(&Ethernet.myNetif, ifinoctets, lenEthFrame);
    if (Ethernet.isUnicast(ethFrame[0]))
    {
        MIB2_STATS_NETIF_INC(&Ethernet.myNetif, ifinucastpkts);
    }
    else
    {
        MIB2_STATS_NETIF_INC(&Ethernet.myNetif, ifinnucastpkts);
    }

    writeRXQueue(&RXQueue, ethFrame, lenEthFrame);
}

#endif

/*******************************************************/

void RF24EthernetClass::tick()
{

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_RP2040) || defined (ARDUINO_ARCH_NRF52) || defined ARDUINO_ARCH_RP2350
    yield();
#elif defined(ARDUINO_ARCH_ESP32)
    const TickType_t xDelay = pdMS_TO_TICKS(1);
    vTaskDelay(xDelay);
#endif

#if USE_LWIP < 1
    uint8_t result = RF24Ethernet.mesh.update();

   if(Ethernet.mesh.mesh_address == 0){
       Ethernet.mesh.DHCP();
   }
   
    if (result == EXTERNAL_DATA_TYPE) {
        if (RF24Ethernet.network.frag_ptr->message_size <= UIP_BUFSIZE && RF24Ethernet.network.frag_ptr->message_size >= 28) {
            uip_len = RF24Ethernet.network.frag_ptr->message_size;
        }
    }
    else if (result == NETWORK_CORRUPTION) {
        RF24Ethernet.networkCorruption++;
    }

    #if !defined(RF24_TAP)
    if (uip_len > 0) {
        uip_input();
        if (uip_len > 0) {
            network_send();
        }
    }
    else if (timer_expired(&Ethernet.periodic_timer)) {
        timer_reset(&Ethernet.periodic_timer);
        for (int i = 0; i < UIP_CONNS; i++) {
            uip_periodic(i);
            /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
            if (uip_len > 0) {
                network_send();
            }
        }
    }
    #else  // defined (RF24_TAP)
    if (uip_len > 0) {
        if (BUF->type == htons(UIP_ETHTYPE_IP)) {
            uip_arp_ipin();
            uip_input();
            /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
            if (uip_len > 0) {
                uip_arp_out();
                network_send();
            }
        }
        else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {
            uip_arp_arpin();
            /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
            if (uip_len > 0) {
                network_send();
            }
        }
    }
    else if (timer_expired(&Ethernet.periodic_timer)) {
        timer_reset(&Ethernet.periodic_timer);
        for (int i = 0; i < UIP_CONNS; i++) {
            uip_periodic(i);
            /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
            if (uip_len > 0) {
                uip_arp_out();
                network_send();
            }
        }
    #endif // defined (RF24_TAP)
    #if UIP_UDP
    for (int i = 0; i < UIP_UDP_CONNS; i++) {
        uip_udp_periodic(i);
        /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
        if (uip_len > 0) {
            // uip_arp_out();
            // network_send();
            RF24UDP::_send((uip_udp_userdata_t*)(uip_udp_conns[i].appstate));
        }
    }
    #endif /* UIP_UDP */
    #if defined(RF24_TAP)
    /* Call the ARP timer function every 10 seconds. */

    if (timer_expired(&Ethernet.arp_timer)) {
        timer_reset(&Ethernet.arp_timer);
        uip_arp_timer();
    }
}
    #endif // RF24_TAP

#else // Using LWIP

    uint8_t result = RF24Ethernet.mesh.update();
    
    if(Ethernet.mesh.mesh_address == 0){
       Ethernet.mesh.DHCP();
    }
   
    if (result == EXTERNAL_DATA_TYPE) {
        if (RF24Ethernet.network.frag_ptr->message_size > 28) {
            uint16_t len = RF24Ethernet.network.frag_ptr->message_size;
            memcpy(networkBuffer,RF24Ethernet.network.frag_ptr->message_buffer,len);
            Ethernet.EthRX_Handler(networkBuffer, len);
            IF_ETH_DEBUG_L1( Serial.println("Net: In"); );
        }
    }

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
         if(useCoreLocking ){ ETHERNET_APPLY_LOCK(); } 
    #endif
    sys_check_timeouts();

    #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
         if(useCoreLocking ){ ETHERNET_REMOVE_LOCK();  } 
    #endif
    
    pbuf* p = readRXQueue(&RXQueue);
    if (p != nullptr)
    {

         #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
         if(useCoreLocking){ ETHERNET_APPLY_LOCK(); } 
        #endif
        if (myNetif.input(p, &myNetif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("IP input error\r\n"));
            pbuf_free(p);
            p = NULL;
        }

        #if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
        if(useCoreLocking){ ETHERNET_REMOVE_LOCK();}
        #endif
    }
    

#endif
}

/*******************************************************/

void RF24EthernetClass::network_send()
{

#if USE_LWIP < 1
    IPAddress gwIP = Ethernet.gatewayIP();
    int16_t nodeAddress = 0;

    //If not the master node
    if (Ethernet.mesh.mesh_address != 0) {
        if (gwIP[3] != uip_buf[19]) {                                       // If not sending to the gateway
            IPAddress local_ip = Ethernet.localIP();
            if(local_ip[0] == uip_buf[16] && local_ip[1] == uip_buf[17]){       // If we are local within the nRF24 network
                //Request an address lookup from the Master node
                nodeAddress = Ethernet.mesh.getAddress((char)uip_buf[19]);  // Do an address lookup
                if (nodeAddress < 0) {
                    nodeAddress = 0;                                    // If the result is negative, send to master
                }
            }                                                           // If this address is outside the nRF24 network, it will be send to master (00)
        }
    }
    else {
        IPAddress local_ip = Ethernet.localIP();
        if(local_ip[0] == uip_buf[16] && local_ip[1] == uip_buf[17]){          // If within the nRF24 radio network, do a lookup, else send to self (00)
            nodeAddress = Ethernet.mesh.getAddress((char)uip_buf[19]);
            if (nodeAddress < 0) {
                return;
            }
        }
    }
    RF24NetworkHeader headerOut(nodeAddress, EXTERNAL_DATA_TYPE);

    #if defined ETH_DEBUG_L1 || defined ETH_DEBUG_L2
    bool ok = RF24Ethernet.network.write(headerOut, uip_buf, uip_len);
    if (!ok) {
        Serial.println();
        Serial.print(millis());
        Serial.println(F(" *** RF24Ethernet Network Write Fail ***"));
    }
    #else
        RF24Ethernet.network.write(headerOut, uip_buf, uip_len);
    #endif

    #if defined ETH_DEBUG_L2
    if (ok) {
        Serial.println();
        Serial.print(millis());
        Serial.println(F(" RF24Ethernet Network Write OK"));
    }
    #endif
#else

#endif
}

/*******************************************************/
/*
void uipudp_appcall() {

}*/
