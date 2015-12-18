
#include "RF24Ethernet.h"

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

uip_userdata_t RF24Client::all_data[UIP_CONNS];


/*************************************************************/

RF24Client::RF24Client(): data(NULL){
}

/*************************************************************/

RF24Client::RF24Client(uip_userdata_t* conn_data) :  data(conn_data){
}

/*************************************************************/

uint8_t RF24Client::connected(){
  return (data && (data->packets_in != 0 || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
}

/*************************************************************/

int RF24Client::connect(IPAddress ip, uint16_t port) {

#if UIP_ACTIVE_OPEN > 0
 
  uint32_t timer = millis();

//do{

  stop();
  uip_ipaddr_t ipaddr;
  uip_ip_addr(ipaddr, ip);

  struct uip_conn* conn = uip_connect(&ipaddr, htons(port));

  if (conn){

	#if UIP_CONNECT_TIMEOUT > 0
    int32_t timeout = millis() + 1000 * UIP_CONNECT_TIMEOUT;
    #endif

    while((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED) {
      RF24EthernetClass::tick();
	
	  if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED) {
	    data = (uip_userdata_t*) conn->appstate;
	    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print(millis()); Serial.print(F(" connected, state: ")); Serial.print(data->state); Serial.print(F(", first packet in: ")); Serial.println(data->packets_in);  );
	    return 1;
	  }	  
	
	#if UIP_CONNECT_TIMEOUT > 0
	  if (((int32_t)(millis() - timeout)) > 0) {
	    conn->tcpstateflags = UIP_CLOSED;
	    break;
	  }
	#endif
    
	}
  }
  //delay(25);
//}while(millis()-timer < 175);

#endif //Active open enabled

return 0;
}

/*************************************************************/

int RF24Client::connect(const char *host, uint16_t port) {
  // Look up the host first
  int ret = 0;

  #if UIP_UDP
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(RF24EthernetClass::_dnsServerAddress);
  ret = dns.getHostByName(host, remote_addr);
  
  if (ret == 1) {
    #if defined (ETH_DEBUG_L1) || #defined (RF24ETHERNET_DEBUG_DNS)
	  Serial.println(F("*UIP Got DNS*"));
	#endif
    return connect(remote_addr, port);
  }
  #endif
  #if defined (ETH_DEBUG_L1) || #defined (RF24ETHERNET_DEBUG_DNS)
    Serial.println(F("*UIP DNS fail*"));
  #endif

  return ret;
}

/*************************************************************/

void RF24Client::stop() {

  if (data && data->state) {
      
	IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print(millis()); Serial.println(F(" before stop(), with data")); );
      
	data->packets_in = 0;
	data->dataCnt = 0;
	  
    if (data->state & UIP_CLIENT_REMOTECLOSED){
      data->state = 0;
    }else{
      data->state |= UIP_CLIENT_CLOSE;
    }
	  
	IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(F("after stop()")); );
    
  }else{
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print(millis()); Serial.println(F(" stop(), data: NULL")); );
  }
	
  data = NULL;
  RF24Ethernet.tick();
}

/*************************************************************/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs) {
  return data && rhs.data && (data == rhs.data);
}

/*************************************************************/

RF24Client::operator bool() {
  Ethernet.tick();
  return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in != 0);
}

/*************************************************************/

size_t RF24Client::write(uint8_t c) {
  return _write(data, &c, 1);
}

/*************************************************************/

size_t RF24Client::write(const uint8_t *buf, size_t size) {
  return _write(data, buf, size);
}

/*************************************************************/

size_t RF24Client::_write(uip_userdata_t* u, const uint8_t *buf, size_t size) {

  size_t total_written = 0;
  size_t payloadSize = rf24_min(size,UIP_TCP_MSS);
  //Serial.println("W1");

  uint32_t testTimeout=millis();
  
test2:    

  RF24EthernetClass::tick(); 	
  if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED ) ) && u->state & (UIP_CLIENT_CONNECTED) ) {

    if( u->out_pos + payloadSize > UIP_TCP_MSS || u->hold){
	  goto test2;
    }

	IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient.write: writePacket(")); Serial.print(u->packets_out); Serial.print(F(") pos: ")); Serial.print(u->out_pos); Serial.print(F(", buf[")); Serial.print(size-total_written); Serial.print(F("]: '")); Serial.write((uint8_t*)buf+total_written,payloadSize); Serial.println(F("'")); );
	
	memcpy(u->myData+u->out_pos,buf+total_written,payloadSize);	
	u->packets_out = 1;
	u->out_pos+=payloadSize;

	total_written += payloadSize;
	
	if( total_written < size ){	
		size_t remain = size-total_written;
		payloadSize = rf24_min(remain,UIP_TCP_MSS);
		
		//RF24EthernetClass::tick();
		goto test2;
	}
	return u->out_pos;
	}
  u->hold = false;
  return -1;
}

/*************************************************************/

void uip_log( char* msg ){
	//Serial.println();
	//Serial.println("** UIP LOG **");
	//Serial.println(msg);
}

/*************************************************************/

void serialip_appcall(void) {
  
  uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;
  
  /*******Connected**********/
  if (!u && uip_connected()){
  
	IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_connected")); );

    u = (uip_userdata_t*) EthernetClient::_allocateData();
    
	if (u) {
      uip_conn->appstate = u;
      IF_RF24ETHERNET_DEBUG_CLIENT( Serial.print(F("UIPClient allocated state: ")); Serial.println(u->state,BIN); );
    }else{
      IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient allocation failed")); );
	}
  }
  
  /*******User Data RX**********/
  if(u){
    if (uip_newdata()){
      IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient uip_newdata, uip_len:")); Serial.println(uip_len); );
      
      if(u->sent){
          u->hold = u->out_pos = u->windowOpened = u->packets_out = false;
      }
	  if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))){
		  
		uip_stop();
		u->state &= ~UIP_CLIENT_RESTART;
		u->windowOpened = false;			
		u->connAbortTime = u->restartTime = millis();
	    memcpy(&u->myDataIn[u->dataPos+u->dataCnt], uip_appdata, uip_datalen());
	    u->dataCnt += uip_datalen();
		
	    u->packets_in = 1;
		  
	  }		  
	  goto finish;
	}

  /*******Closed/Timed-out/Aborted**********/  
  // If the connection has been closed, save received but unread data.
  if (uip_closed() || uip_timedout() || uip_aborted()) {
    
	IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println();Serial.print(millis()); Serial.println(F(" UIPClient uip_closed")); );
	// drop outgoing packets not sent yet:
	u->packets_out = 0;
    
	if (u->packets_in) {
	  ((uip_userdata_closed_t *)u)->lport = uip_conn->lport;
	  u->state |= UIP_CLIENT_REMOTECLOSED;
	  IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(F("UIPClient close 1")); );
	}else{
	  IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(F("UIPClient close 2")); );
	  u->state = 0;
	}

	IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(F("after UIPClient uip_closed")); );
	uip_conn->appstate = NULL;	
	goto finish;
  }

  /*******ACKED**********/  
  if (uip_acked()) {
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_acked")); );
	u->state &= ~UIP_CLIENT_RESTART;
	u->hold = u->out_pos = u->windowOpened = u->packets_out = false;
	u->connAbortTime = u->restartTime = millis();	
  }
	
  /*******Polling**********/
  if (uip_poll() || uip_rexmit() ){
    //IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.println(F("UIPClient uip_poll")); );
    
	if (u->packets_out != 0 ) {
	  uip_len = u->out_pos;
	  uip_send(u->myData,u->out_pos);
	  u->hold = true;
      u->sent = true;
	  goto finish;    
	}else
    // Restart mechanism to keep connections going
	// Only call this if the TCP window has already been re-opened, the connection is being polled, but no data
	// has been acked
	if( !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)) ){
	  
	  if( u->windowOpened == true && u->state & UIP_CLIENT_RESTART && millis() - u->restartTime > u->restartInterval ){		
		u->restartTime = millis();		    
		// Abort the connection if the connection is dead after a set timeout period (uip-conf.h)
		#if defined UIP_CONNECTION_TIMEOUT
		
		if(millis() - u->connAbortTime >= UIP_CONNECTION_TIMEOUT){
		  #if defined RF24ETHERNET_DEBUG_CLIENT || defined ETH_DEBUG_L1
		  Serial.println(); Serial.print(millis()); Serial.println(F(" *********** ABORTING CONNECTION ***************"));
		  #endif
		  u->windowOpened = false;
		  u->state = 0;
		  uip_conn->appstate = NULL;
		  uip_abort();
		  goto finish;	      
		}else{
		#endif
		#if defined RF24ETHERNET_DEBUG_CLIENT || defined ETH_DEBUG_L1
		  Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient Re-Open TCP Window, time remaining before abort: ")); Serial.println((UIP_CONNECTION_TIMEOUT - (millis() - u->connAbortTime)) / 1000.00);
		#endif
		  u->restartInterval+=500;
		  u->restartInterval=rf24_min(u->restartInterval,7000);
		  uip_restart();
		#if defined UIP_CONNECTION_TIMEOUT
	    }
		#endif
	  }
	}
  }


  /*******Close**********/	
	
  if (u->state & UIP_CLIENT_CLOSE) {
    IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient state UIP_CLIENT_CLOSE")); );

	if (u->packets_out == 0) {
      u->state = 0;
      uip_conn->appstate = NULL;
      uip_close();
      IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(F("no blocks out -> free userdata")); );

    }else{
      uip_stop();
      IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(F("blocks outstanding transfer -> uip_stop()")); );
    }
  }			
finish:;
finish_newdata:
  if (u->state & UIP_CLIENT_RESTART && !u->windowOpened) {
    if( !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))){	  
	  uip_restart();
	  #if defined ETH_DEBUG_L1
	    Serial.println(); Serial.print(millis()); 
		Serial.println(F(" UIPClient Re-Open TCP Window"));
      #endif
	  u->windowOpened = true;
	  u->restartInterval = UIP_WINDOW_REOPEN_DELAY; //.75 seconds
	  u->restartTime = u->connAbortTime = millis();
	}
  }
  }
  


}

/*******************************************************/


uip_userdata_t *RF24Client::_allocateData() {
  for ( uint8_t sock = 0; sock < UIP_CONNS; sock++ ) {
	uip_userdata_t* data = &RF24Client::all_data[sock];
    if (!data->state) {
	  data->state = sock | UIP_CLIENT_CONNECTED;
	  data->packets_in=0;
	  data->packets_out=0;
	  data->dataCnt = 0;
	  data->dataPos=0;
	  data->out_pos = 0;
      data->hold=0;
	  return data;
	}
  }
  return NULL;
}

int RF24Client::waitAvailable(uint32_t timeout){

	uint32_t start = millis();
    while(available() < 1){	
		if(millis()-start > timeout){
		  return 0; 
		}
		RF24Ethernet.tick();
	}
	return available();
}

/*************************************************************/

int RF24Client::available() {
  
  RF24Ethernet.tick();    
  if (*this){	
	return _available(data);
  }
  return 0;
}

/*************************************************************/

int RF24Client::_available(uip_userdata_t *u) {
  if(u->packets_in){
	return u->dataCnt;
  }
  return 0;
}

int RF24Client::read(uint8_t *buf, size_t size) {

  if (*this) {

    if (!data->packets_in) { return -1; }

    size = rf24_min(data->dataCnt,size);
	memcpy(buf,&data->myDataIn[data->dataPos],size);
	data->dataCnt -= size;
	
	data->dataPos+=size;
	
	if(!data->dataCnt) {
      
	  data->packets_in = 0;
	  data->dataPos = 0;
	  
      if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) && !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))){
        data->state |= UIP_CLIENT_RESTART;
		data->restartTime = 0;
		
		IF_ETH_DEBUG_L2( Serial.print(F("UIPClient set restart ")); Serial.println(data->state & UIP_CLIENT_SOCKETS); Serial.println(F("**")); Serial.println(data->state,BIN); Serial.println(F("**")); Serial.println(UIP_CLIENT_SOCKETS,BIN); Serial.println(F("**"));  );
	  }else{
		IF_ETH_DEBUG_L2( Serial.print(F("UIPClient stop?????? ")); Serial.println(data->state & UIP_CLIENT_SOCKETS); Serial.println(F("**")); Serial.println(data->state,BIN); Serial.println(F("**")); Serial.println(UIP_CLIENT_SOCKETS,BIN); Serial.println(F("**"));  );
			  
	  }
	  
      if (data->packets_in == 0) {
        if (data->state & UIP_CLIENT_REMOTECLOSED) {
          data->state = 0;
          data = NULL;
        }        
      }
    }
    return size;
  }
  
  return -1;
}

/*************************************************************/

int RF24Client::read() {
  uint8_t c;
  if (read(&c,1) < 0)
    return -1;
  return c;
}

/*************************************************************/

int RF24Client::peek() {
	if(available()){
	  return data->myDataIn[data->dataPos];
	}
  return -1;
}

/*************************************************************/

void RF24Client::flush() {
  if (*this) {
	data->packets_in = 0;
	data->dataCnt = 0;
  }
}

/*************************************************************/
