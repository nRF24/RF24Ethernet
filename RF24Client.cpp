
extern "C"
{
#import "uip-conf.h"
#import "utility/uip.h"
#import "utility/uip_arp.h"
//#import "string.h"
}

#include "RF24Ethernet.h"
#include "RF24Client.h"
//#include "Dns.h"
#include "RF24Network.h"

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

uip_userdata_t RF24Client::all_data[UIP_CONNS];


/*************************************************************/

RF24Client::RF24Client():
	data(NULL)
{}

/*************************************************************/

RF24Client::RF24Client(uip_userdata_t* conn_data) :
    data(conn_data)
{
}

/*************************************************************/

uint8_t RF24Client::connected(){
	return (data && (data->packets_in[0] != 0 || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
}

/*************************************************************/

int RF24Client::connect(IPAddress ip, uint16_t port)
{

  stop();
  uip_ipaddr_t ipaddr;
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
    }
  return 0;
}

/*************************************************************/

int
RF24Client::connect(const char *host, uint16_t port)
{
  // Look up the host first
  int ret = 0;
  //Serial.println("conn dns");
#if UIP_UDP
  DNSClient dns;
  IPAddress remote_addr;

   //Serial.println(RF24EthernetClass::_dnsServerAddress[0]);
  dns.begin(RF24EthernetClass::_dnsServerAddress);
  ret = dns.getHostByName(host, remote_addr);
  
  if (ret == 1) {
	//Serial.println("got dns");
    return connect(remote_addr, port);
  }
#endif
  //Serial.println("return");
  //Serial.println(ret,DEC);
  return ret;
}

/*************************************************************/

void RF24Client::stop()
{
  if (data && data->state)
    {
#ifdef RF24ETHERNET_DEBUG_CLIENT
      Serial.println(F("before stop(), with data"));
      //_dumpAllData();
#endif
       data->packets_in[0] = 0;
      //_flushBlocks(&data->packets_in[0]);
      if (data->state & UIP_CLIENT_REMOTECLOSED)
        {
          data->state = 0;
        }
      else
        {
          data->state |= UIP_CLIENT_CLOSE;
        }
#ifdef RF24ETHERNET_DEBUG_CLIENT
      Serial.println(F("after stop()"));
      //_dumpAllData();
#endif
    }
#ifdef RF24ETHERNET_DEBUG_CLIENT
  else
    {
      Serial.println(F("stop(), data: NULL"));
    }
#endif
  data = NULL;
  RF24Ethernet.tick();
}

/*************************************************************/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs)
{
  return data && rhs.data && (data = rhs.data);
}

/*************************************************************/

RF24Client::operator bool()
{
  Ethernet.tick();
  //return RF24Ethernet.dataCnt;
  return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in[0] != 0);
}

/*************************************************************/

size_t RF24Client::write(uint8_t c)
{
  return _write(data, &c, 1);
	//return 0;
}

/*************************************************************/

size_t RF24Client::write(const uint8_t *buf, size_t size)
{
  return _write(data, buf, size);
}

/*************************************************************/

size_t RF24Client::_write(uip_userdata_t* u, const uint8_t *buf, size_t size)
{

test:	  
	  RF24EthernetClass::tick();
	  if(u->packets_out[0] == 1){
		//delay(50);
		goto test;
	  }
  RF24NetworkHeader headerOut(00,EXTERNAL_DATA_TYPE);
  int remain = size;
  uint16_t written;
#if UIP_ATTEMPTS_ON_WRITE > 0
  uint16_t attempts = UIP_ATTEMPTS_ON_WRITE;
#endif
  repeat:
  
  RF24EthernetClass::tick();
  if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
    {	


#ifdef RF24ETHERNET_DEBUG_CLIENT
      Serial.print(F("UIPClient.write: writePacket("));
      Serial.print(u->packets_out[p]);
      Serial.print(F(") pos: "));
      Serial.print(u->out_pos);
      Serial.print(F(", buf["));
      Serial.print(size-remain);
      Serial.print(F("-"));
      Serial.print(remain);
      Serial.print(F("]: '"));
      Serial.write((uint8_t*)buf+size-remain,remain);
      Serial.println(F("'"));
#endif
    	
	 memcpy(&u->myDataOut[0],buf,size);
	 u->packets_out[0] = 1;
     remain -= size;//written;
     u->out_pos=size;
	  //Serial.print(F(") pos: "));
      //Serial.println(u->out_pos);
/*	  
test2:	  
	  RF24EthernetClass::tick();
	  if(u->packets_out[0] == 1){
		goto test2;
	  }*/
	  //u->packets_out[0] = 1;
      /*if (remain > 0)
        {
          if (p == UIP_SOCKET_NUMPACKETS-1)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
              if ((--attempts)>0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                goto repeat;
#endif
              goto ready;
            }
          p++;
          goto newpacket;
        }*/
ready:
/*#if UIP_CLIENT_TIMER >= 0
      u->timer = millis()+UIP_CLIENT_TIMER;
#endif*/
      return size;//-remain;
    }
  return -1;
}

/*************************************************************/

void serialip_appcall(void)
{

  uint16_t send_len = 0;
  uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;
  
  if (!u && uip_connected()){
#ifdef RF24ETHERNET_DEBUG_CLIENT
      Serial.println(F("UIPClient uip_connected"));
  //    RF24Client::_dumpAllData();
#endif
     u = (uip_userdata_t*) EthernetClient::_allocateData();
      if (u)
        {
          uip_conn->appstate = u;
#ifdef RF24ETHERNET_DEBUG_CLIENT
          Serial.print(F("UIPClient allocated state: "));
          Serial.println(u->state,BIN);
#endif
        }
#ifdef RF24ETHERNET_DEBUG_CLIENT
      else
        Serial.println(F("UIPClient allocation failed"));
#endif
    }
  
    if(u){
      if (uip_newdata()){
#ifdef RF24ETHERNET_DEBUG_CLIENT
          Serial.print(F("UIPClient uip_newdata, uip_len:"));
          Serial.println(uip_len);
#endif
		u->state &= ~UIP_CLIENT_RESTART;
		u->restartTime = millis();
        if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))){
            uip_stop();
			RF24Ethernet.dataCnt = uip_datalen();
		    memcpy(&RF24Ethernet.myData,uip_appdata,RF24Ethernet.dataCnt);
			u->packets_in[0] = 1;
		}	  
	  }
	}
	
finish_newdata:
    if (u->state & UIP_CLIENT_RESTART && millis() - u->restartTime > 2500)
    {	  
      
	  if( !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))){
		
		/*if(!uip_stopped(uip_conn)){
			Serial.print("not stopped");
		}*/
			uip_restart();

	  // Workaround to prevent the connection from stalling when a restart packet fails to get through
	  // If data has not been received by the next time round, call restart again
		u->restartTime = millis();
		//Serial.print("rst");
		
	  }
	  
    }
	
	
	      // If the connection has been closed, save received but unread data.
      if (uip_closed() || uip_timedout() || uip_aborted())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient uip_closed"));
          RF24Client::_dumpAllData();
#endif
          // drop outgoing packets not sent yet:
          //UIPClient::_flushBlocks(&u->packets_out[0]);
          if (u->packets_in[0] != 0)
            {
              ((uip_userdata_closed_t *)u)->lport = uip_conn->lport;
              u->state |= UIP_CLIENT_REMOTECLOSED;
            }
          else{
            u->state = 0;
		  }
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("after UIPClient uip_closed"));
          //RF24Client::_dumpAllData();
#endif
          uip_conn->appstate = NULL;
          goto finish;
        }
		
	      if (uip_acked())
        {
#ifdef RF24ETHERNET_DEBUG_CLIENT
            Serial.println(F("UIPClient uip_acked"));
#endif
			u->packets_out[0] = 0;
			u->out_pos=0;
         // UIPClient::_eatBlock(&u->packets_out[0]);
			
        }
	if (uip_poll() )
        {
#ifdef RF24ETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient uip_poll"));
#endif
          if (u->packets_out[0] != 0 )
            {
                  send_len = u->out_pos;

              if (send_len > 0)
                {
                  RF24Ethernet.uip_hdrlen = ((uint8_t*)uip_appdata)-uip_buf;
					   uip_len = send_len;
					   uip_send(&u->myDataOut[0],send_len);
					   
                      RF24Ethernet.packetstate |= UIPETHERNET_SENDPACKET;
                }
              goto finish;
            }
        }
	 if( uip_rexmit()){
	     //Serial.println("REXMIT");
		if (u->packets_out[0] != 0 )
            {
              send_len = u->out_pos;

              if (send_len > 0)
                {
                  RF24Ethernet.uip_hdrlen = ((uint8_t*)uip_appdata)-uip_buf;
				  uip_len = send_len;
				  uip_send(&u->myDataOut[0],send_len);
                  RF24Ethernet.packetstate |= UIPETHERNET_SENDPACKET;
                }
              goto finish;
            } 
	 }
		
		
	
	      if (u->state & UIP_CLIENT_CLOSE)
        {
#ifdef RF24ETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient state UIP_CLIENT_CLOSE"));
          //UIPClient::_dumpAllData();
#endif
		  if (u->packets_out[0] == 0)
            {
              u->state = 0;
              uip_conn->appstate = NULL;
              uip_close();
#ifdef RF24ETHERNET_DEBUG_CLIENT
              Serial.println(F("no blocks out -> free userdata"));
              //UIPClient::_dumpAllData();
#endif
            }
          else
            {
              uip_stop();
#ifdef RF24ETHERNET_DEBUG_CLIENT
              Serial.println(F("blocks outstanding transfer -> uip_stop()"));			  
#endif
            }
        }			

  finish:;
  //uip_send(uip_appdata,send_len);
  //uip_len = send_len;
  //u->packets_out[0]=0;


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
          //memset(&data->packets_in[0],0,sizeof(uip_userdata_t)-sizeof(data->state));
		  data->packets_in[0]=0;
          return data;
        }
    }
  return NULL;
}

/*************************************************************/

int RF24Client::available()
{
  if (*this)
	RF24Ethernet.tick();
    return _available(data);
  return 0;
}

/*************************************************************/

int RF24Client::_available(uip_userdata_t *u)
{
  int len = 0;
  //for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
   // {
      //len += Enc28J60Network::blockSize(u->packets_in[i]);
	  
  //}
  len = RF24Ethernet.dataCnt;
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
		  size = min(RF24Ethernet.dataCnt,size);
		  memcpy(buf,&RF24Ethernet.myData,size);
		  RF24Ethernet.dataCnt -= size;
		  memmove(RF24Ethernet.myData,RF24Ethernet.myData+size,RF24Ethernet.dataCnt);	  			
		  //Serial.println("got ext");	  

		  read = size;
         if(!RF24Ethernet.dataCnt)// read == Enc28J60Network::blockSize(data->packets_in[0]))
         {
             // remain -= read;
			 remain = 0;
			  data->packets_in[0] = 0;
              //_eatBlock(&data->packets_in[0]);
              if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) && !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
                data->state |= UIP_CLIENT_RESTART;
				data->restartTime = 0;
              if (data->packets_in[0] == 0)
                {
                  if (data->state & UIP_CLIENT_REMOTECLOSED)
                    {
                      data->state = 0;
                      data = NULL;
                    }
                  return size-remain;
                }
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

/*************************************************************/

int RF24Client::read()
{
  uint8_t c;
  if (read(&c,1) < 0)
    return -1;
  return c;
}

/*************************************************************/

int RF24Client::peek()
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

/*************************************************************/

void RF24Client::flush()
{
  if (*this)
    {
      //_flushBlocks(&data->packets_in[0]);
    }
}

/*************************************************************/
