

#ifndef RF24CLIENT_H
#define RF24CLIENT_H


#include "ethernet_comp.h"
#include "Print.h"
#import "Client.h"

extern "C" {
  #import "utility/uip.h"
}


#define UIP_SOCKET_DATALEN UIP_TCP_MSS
//#define UIP_SOCKET_NUMPACKETS UIP_RECEIVE_WINDOW/UIP_TCP_MSS+1
#ifndef UIP_SOCKET_NUMPACKETS
#define UIP_SOCKET_NUMPACKETS 5
#endif

#define UIP_CLIENT_CONNECTED 0x10
#define UIP_CLIENT_CLOSE 0x20
#define UIP_CLIENT_REMOTECLOSED 0x40
#define UIP_CLIENT_RESTART 0x80
#define UIP_CLIENT_STATEFLAGS (UIP_CLIENT_CONNECTED | UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED | UIP_CLIENT_RESTART)
#define UIP_CLIENT_SOCKETS ~UIP_CLIENT_STATEFLAGS



typedef struct {
  uint8_t state;
  uint8_t packets_in[UIP_SOCKET_NUMPACKETS];
  uint16_t lport;        /**< The local TCP port, in network byte order. */
} uip_userdata_closed_t;

/*
typedef struct {
  uint8_t state;
  memhandle packets_in[UIP_SOCKET_NUMPACKETS];
  memhandle packets_out[UIP_SOCKET_NUMPACKETS];
  memaddress out_pos;
#if UIP_CLIENT_TIMER >= 0
  unsigned long timer;
#endif
} uip_userdata_t;
*/


typedef struct {
  uint8_t state;
  uint8_t packets_in[UIP_SOCKET_NUMPACKETS];
  uint8_t packets_out[UIP_SOCKET_NUMPACKETS];
  uint8_t out_pos;
#if UIP_CLIENT_TIMER >= 0
  unsigned long timer;
#endif
 uint8_t connected;
} uip_userdata_t;




class RF24Client : public Client {

public:
	RF24Client();
	//RF24Client(int len);
	//RF24Client(uint8_t sock);
	int connect(IPAddress ip, uint16_t port);
    int connect(const char *host, uint16_t port);
    int read(uint8_t *buf, size_t size);
    void stop();
  
  
	uint8_t connected();
	//int available();
	
  size_t write(uint8_t);
  size_t write(const uint8_t *buf, size_t size);
  int available();
  int read();
  int peek();
  void flush();

  using Print::write;	
		
  operator bool();
  virtual bool operator==(const EthernetClient&);
  virtual bool operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };
  
	
private:
	uint8_t _sock;
	uint16_t _dataLen;
	
	static bool isConnected;
	static bool clientClose;
	static bool readData;
	
	static int handle_connection(uip_tcp_appstate_t *s);
    //This is referenced by UIPServer.cpp in available()
    //RF24Client(serialip_state* conn_data);
	//RF24Client(uint8_t sock);
	//RF24Client(size_t tmp);
    RF24Client(struct uip_conn *_conn);
    RF24Client(uip_userdata_t* conn_data);
	//RF24Client(serialip_state* conn_data);

	
    uip_userdata_t* data;
	
	static int _available(uip_userdata_t *);
	
    static uip_userdata_t all_data[UIP_CONNS];
	static uip_userdata_t* _allocateData();
	 
	friend class RF24EthernetClass;
	friend class RF24Server;
	
	friend void uipclient_appcall(void);	
	friend void serialip_appcall(void);
	
};


#endif