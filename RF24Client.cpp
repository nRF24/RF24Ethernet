
extern "C"
{
#import "uip-conf.h"
#import "utility/uip.h"
#import "utility/uip_arp.h"
//#import "string.h"
}

#include "RF24Ethernet.h"
#include "RF24Client.h"

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

uip_userdata_t RF24Client::all_data[UIP_CONNS];


RF24Client::RF24Client():
	data(NULL)
{}

//RF24Client::RF24Client(uint16_t dataSize): _dataLen(dataSize){}

//EthernetClient::EthernetClient(uint8_t sock) : _sock(sock) {
//}


RF24Client::RF24Client(uip_userdata_t* conn_data) :
    data(conn_data)
{
}

uint8_t RF24Client::connected(){
	return 1;//(uip_connected());
}

/*
int RF24Client::available(){
	return RF24Ethernet.dataCnt;
}*/



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

void RF24Client::stop()
{
  if (data && data->state)
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
      //_dumpAllData();
#endif
    }
#ifdef UIPETHERNET_DEBUG_CLIENT
  else
    {
      Serial.println(F("stop(), data: NULL"));
    }
#endif
  data = NULL;
  RF24Ethernet.tick();
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
  //return 0;//!RF24Ethernet.dataCnt;
  return data && rhs.data && (data = rhs.data);
  //return 0;
}

RF24Client::operator bool()
{
  Ethernet.tick();
  //return RF24Ethernet.dataCnt;
  return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in[0] != 0);
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


void serialip_appcall(void)
{


  uint16_t send_len = 0;
  uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;
  
  if (!u && uip_connected()){
//#ifdef RF24ETHERNET_DEBUG_CLIENT
      Serial.println(F("UIPClient uip_connected"));
  //    RF24Client::_dumpAllData();
//#endif
     u = (uip_userdata_t*) EthernetClient::_allocateData();
      if (u)
        {
          uip_conn->appstate = u;
//#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.print(F("UIPClient allocated state: "));
          Serial.println(u->state,BIN);
//#endif
        }
//#ifdef UIPETHERNET_DEBUG_CLIENT
      else
        Serial.println(F("UIPClient allocation failed"));
//#endif
    }
  
    if(u){
      if (uip_newdata()){
//#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.print(F("UIPClient uip_newdata, uip_len:"));
          Serial.println(uip_len);
//#endif

        if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))){
             // for (uint8_t i=0; i < UIP_SOCKET_NUMPACKETS; i++){
             //     if (u->packets_in[i] == NOBLOCK){
             //         u->packets_in[i] = !NOBLOCK;//Enc28J60Network::allocBlock(uip_len);
             //         if (u->packets_in[i] != NOBLOCK){
			 Serial.println("Data buffered");
			RF24Ethernet.dataCnt = min(UIP_BUFSIZE-42,uip_datalen());
		    memcpy(&RF24Ethernet.myData,uip_appdata,RF24Ethernet.dataCnt);
			u->packets_in[0] = 1;
			//RF24EthernetClass::packetstate &= ~UIPETHERNET_FREEPACKET;
		}
	  }
	}
	      if (u->state & UIP_CLIENT_CLOSE)
        {
//#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient state UIP_CLIENT_CLOSE"));
          //UIPClient::_dumpAllData();
//#endif
          //if (u->packets_out[0] == 0)
		  if (u->packets_out[0] == 0)
            {
              u->state = 0;
              uip_conn->appstate = NULL;
              uip_close();
//#ifdef UIPETHERNET_DEBUG_CLIENT
              Serial.println(F("no blocks out -> free userdata"));
              //UIPClient::_dumpAllData();
//#endif
            }
          else
            {
              uip_stop();
//#ifdef UIPETHERNET_DEBUG_CLIENT
              Serial.println(F("blocks outstanding transfer -> uip_stop()"));
//#endif
            }
        }					
						

/*	struct serialip_state *s = &(uip_conn->appstate);
	//uip_listen(HTONS(port));
	//struct serialip_state *t = &(uip_conn->appstate);
	
	//uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;
	if (uip_connected()) {
		//Serial.print("Conn on port: ");
		//Serial.println(HTONS(uip_conn->lport));
		s->connected = 1;		
		//PSOCK_INIT(&s->p, s->inputBuffer, sizeof(s->inputBuffer));	
	}
	
	if(uip_newdata()){
		RF24Ethernet.dataCnt = min(UIP_BUFSIZE-42,uip_datalen());
		memcpy(&RF24Ethernet.myData,uip_appdata,RF24Ethernet.dataCnt);
		s->packets_in[0] = 1;
		//if(PSOCK_NEWDATA(&s->p)) {
			//If data is available, put it into the input buffer
			//PSOCK_READTO(&s->p, '\n');
			//memcpy(s->outputBuffer,s->inputBuffer,PSOCK_DATALEN(s));
	//	}
	}
	
	if (uip_closed() || uip_aborted() || uip_timedout() || uip_rexmit() ) {
		uip_close();
		#if defined (ETH_DEBUG_L1)
		Serial.println("*****CLOSE *********");
		#endif
		//PSOCK_CLOSE(&s->p);
		//PSOCK_END(&s->p);
	}
	
	RF24Client::handle_connection(s);*/
}

/*******************************************************/

 int RF24Client::handle_connection(uip_tcp_appstate_t *s){
/*		PSOCK_BEGIN(&s->p);
		
		
	//Serial.println("psock connection begin");		
		
  PSOCK_SEND_STR(&s->p, "HTTP/1.1 200 OK\n");
  PSOCK_SEND_STR(&s->p, "Content-Type: text/html\n");
  //PSOCK_SEND_STR(&s->p, "Connection: close\n");
  PSOCK_SEND_STR(&s->p, "Refresh: 5\n");
  PSOCK_SEND_STR(&s->p, "\n");
  PSOCK_SEND_STR(&s->p, "<!DOCTYPE HTML>\n");
  PSOCK_SEND_STR(&s->p, "<html>\n");
  PSOCK_SEND_STR(&s->p, "HELLO FROM ARDUINO!\n");
  PSOCK_SEND_STR(&s->p, "</html>\n");
  PSOCK_READTO(&s->p, '\n');
  
  PSOCK_CLOSE(&s->p);
  PSOCK_END(&s->p);*/
}

/*******************************************************/


uip_userdata_t *RF24Client::_allocateData()
{
  for ( uint8_t sock = 0; sock < UIP_CONNS; sock++ )
    {
      uip_userdata_t* data = &RF24Client::all_data[sock];
      if (!data->state)
        {
          data->state = sock | UIP_CLIENT_CONNECTED;
          memset(&data->packets_in[0],0,sizeof(uip_userdata_t)-sizeof(data->state));
          return data;
        }
    }
  return NULL;
}

/*
size_t RF24Client::_write(uip_userdata_t* u, const uint8_t *buf, size_t size)
{
  return 0;
}*/


int RF24Client::available()
{
  if (*this)
	//return RF24Ethernet.dataCnt;
    return _available(data);
  return 0;
}

int RF24Client::_available(uip_userdata_t *u)
{
  int len = 0;
  //for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
   // {
      //len += Enc28J60Network::blockSize(u->packets_in[i]);
	  
  //}
  //if(UIPEthernet.network.update() == EXTERNAL_DATA_TYPE){
 // RF24NetworkFrame *frame = UIPEthernet.network.frag_ptr;
 // len = frame->message_size;
 // }

	len = RF24Ethernet.dataCnt;
//  if(UIPEthernet.inPos > 0){
//	len=UIPEthernet.inPos;
//  }
  //len=UIPEthernet.dataSize;
  //Serial.print("av len");
  //Serial.println(len);
  return len;
}

int RF24Client::read(uint8_t *buf, size_t size)
{

//Serial.println("read");
  if (*this)
    {
      uint16_t remain = size;
      if (data->packets_in[0] == 0)
        return 0;
      uint16_t read = 0;
      //do
      //  {
          //read = Enc28J60Network::readPacket(data->packets_in[0],0,buf+size-remain,remain);
/*		  Serial.println("1");
		  RF24NetworkFrame *frame = UIPEthernet.network.frag_ptr;
	      read = frame->message_size;	  
          //Serial.println("got ext");
		  memcpy(&buf,&frame->message_buffer,size);*/
		  //read = UIPEthernet.inPos;
		  //memcpy(&buf,&UIPEthernet.myPackets_In,remain);
		  size = min(RF24Ethernet.dataCnt,size);
		  memcpy(buf,&RF24Ethernet.myData,size);
		  RF24Ethernet.dataCnt -= size;
		  memmove(RF24Ethernet.myData,RF24Ethernet.myData+size,RF24Ethernet.dataCnt);	  
			
		  
		  
		  //memcpy(&buf,UIPEthernet.packetDataIn,size);
		  read = size;
         if(!RF24Ethernet.dataCnt)// read == Enc28J60Network::blockSize(data->packets_in[0]))
         {
              remain -= read;
			  data->packets_in[0] = 0;
              //_eatBlock(&data->packets_in[0]);
              if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) && !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
                data->state |= UIP_CLIENT_RESTART;
             // if (data->packets_in[0] == NOBLOCK)
             //   {
                  if (data->state & UIP_CLIENT_REMOTECLOSED)
                    {
                      data->state = 0;
                      data = NULL;
                    }
                  return size-remain;
             //   }
        }
          /*else
            {
              Enc28J60Network::resizeBlock(data->packets_in[0],read);
              break;
            }*/
      //  }
      //while(remain > 0);
      return size;
    }
  return -1;
}

int
RF24Client::read()
{
	//memcpy(buf,&RF24Ethernet.myData,size);
	//RF24Ethernet.dataCnt -= size;
	
  uint8_t c;
  if (read(&c,1) < 0)
    return -1;
  return c;
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