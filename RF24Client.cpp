
extern "C"
{
//#import "utility/uip-conf.h"
#import "utility/uip.h"
#import "utility/uip_arp.h"
//#import "string.h"
}

#include "RF24Ethernet.h"
#include "RF24Client.h"

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

uip_userdata_t RF24Client::all_data[UIP_CONNS];


RF24Client::RF24Client(){}

RF24Client::RF24Client(uint16_t dataSize): _dataLen(dataSize){}

EthernetClient::EthernetClient(uint8_t sock) : _sock(sock) {
}

//RF24Client::RF24Client(uip_userdata_t* conn_data) :
 //   data(conn_data)
//{
//}

uint8_t RF24Client::connected(){
	return 1;//(uip_connected());
}

int RF24Client::available(){
	return RF24Ethernet.dataCnt;
}



int RF24Client::connect(IPAddress ip, uint16_t port)
{
  //stop();
  /*uip_ipaddr_t ipaddr;
  uip_ip_addr(ipaddr, ip);
  struct uip_conn* conn = uip_connect(&ipaddr, htons(port));
  if (conn)
    {
#if UIP_CONNECT_TIMEOUT > 0
      int32_t timeout = millis() + 1000 * UIP_CONNECT_TIMEOUT;
#endif
      while((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED)
        {
          RF24EthernetClass::tick();
          if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED)
            {
              data = (uip_userdata_t*) conn->appstate;
#ifdef UIPETHERNET_DEBUG_CLIENT
              Serial.print(F("connected, state: "));
              Serial.print(data->state);
              Serial.print(F(", first packet in: "));
              Serial.println(data->packets_in[0]);
#endif
              return 1;
            }
#if UIP_CONNECT_TIMEOUT > 0
          if (((int32_t)(millis() - timeout)) > 0)
            {
              conn->tcpstateflags = UIP_CLOSED;
              break;
            }
#endif
        }
    }*/
  return 1;
}

int
RF24Client::connect(const char *host, uint16_t port)
{
  // Look up the host first
  int ret = 1;
/*#if UIP_UDP
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(UIPEthernetClass::_dnsServerAddress);
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return connect(remote_addr, port);
  }
#endif*/
  return ret;
}

void
RF24Client::stop()
{
  /*if (data && data->state)
    {
#ifdef UIPETHERNET_DEBUG_CLIENT
      Serial.println(F("before stop(), with data"));
      _dumpAllData();
#endif
      //_flushBlocks(&data->packets_in[0]);
      if (data->state & UIP_CLIENT_REMOTECLOSED)
        {
          data->state = 0;
        }
      else
        {
          data->state |= UIP_CLIENT_CLOSE;
        }
#ifdef UIPETHERNET_DEBUG_CLIENT
      Serial.println(F("after stop()"));
      _dumpAllData();
#endif
    }
#ifdef UIPETHERNET_DEBUG_CLIENT
  else
    {
      Serial.println(F("stop(), data: NULL"));
    }
#endif
  data = NULL;*/
  //RF24EthernetClass::tick();
}
/*
uint8_t RF24Client::connected()
{
  //return (data && (data->packets_in[0] != NOBLOCK || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
  return 0;
}*/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs)
{
  return RF24Ethernet.dataCnt;//uip_connected();//data && rhs.data && (data == rhs.data);
  //return 0;
}

RF24Client::operator bool()
{
  //RF24EthernetClass::tick();
  return RF24Ethernet.dataCnt;//uip_connected();//data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in[0] != NOBLOCK);
}

size_t RF24Client::write(uint8_t c)
{
  //return _write(data, &c, 1);
	return 0;
}

size_t RF24Client::write(const uint8_t *buf, size_t size)
{
  return 0;//_write(data, buf, size);
}
/*
size_t RF24Client::_write(uip_userdata_t* u, const uint8_t *buf, size_t size)
{
  return 0;
}*/

/*
int RF24Client::available()
{
  if (*this)
    return _available(data);
  return 0;
}*/
/*
int RF24Client::_available(uip_userdata_t *u)
{
  int len = 0;
//  for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
//    {
//      len += Enc28J60Network::blockSize(u->packets_in[i]);
//  }
//  if(UIPEthernet.network.update() == EXTERNAL_DATA_TYPE){
//  RF24NetworkFrame *frame = UIPEthernet.network.frag_ptr;
//  len = frame->message_size;
//  }

//  if(UIPEthernet.inPos > 0){
//	len=UIPEthernet.inPos;
//  }
  //len=UIPEthernet.dataSize;
  //Serial.print("av len");
  //Serial.println(len);
  return len;
}
*/
int RF24Client::read(uint8_t *buf, size_t size)
{
}

int
RF24Client::read()
{/*
  uint8_t c;
  if (read(&c,1) < 0)
    return -1;
  return c;*/
}

int
RF24Client::peek()
{
  if (*this)
    {
     /*if (data->packets_in[0] != NOBLOCK)
        {
          uint8_t c;
          Enc28J60Network::readPacket(data->packets_in[0],0,&c,1);
          return c;
        }*/
    }
  return -1;
}

void RF24Client::flush()
{
  if (*this)
    {
      //_flushBlocks(&data->packets_in[0]);
    }
}