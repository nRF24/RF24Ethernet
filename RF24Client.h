

#ifndef RF24CLIENT_H
#define RF24CLIENT_H

#include "Print.h"
#import "Client.h"

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
  uint16_t lport;        /**< The local TCP port, in network byte order. */
} uip_userdata_closed_t;


/**
 * Data structure for holding per connection data
 * @warning <b> This is used internally and should not be accessed directly by users </b>
 */
typedef struct {
  uint8_t state;
  bool packets_in;
  bool packets_out;
  uint16_t out_pos;
#if UIP_CLIENT_TIMER >= 0
  unsigned long timer;
#endif
 bool windowOpened;
 uint32_t restartTime;
 uint32_t restartInterval;
 uint32_t connAbortTime;
 uint8_t myData[OUTPUT_BUFFER_SIZE];
 uint8_t myDataIn[OUTPUT_BUFFER_SIZE]; 
 uint16_t dataPos;
 uint16_t dataCnt;
 bool hold;
 bool sent;
} uip_userdata_t;




class RF24Client : public Client {

public:

	/**
	* Basic constructor
	*/	
	RF24Client();
	
	/**
	* Establish a connection to a specified IP address and port
	*/
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
	
	/**
	* Disconnects from the current active connection
	*/
    void stop();  
  
	/**
	* Indicates whether the client is connected or not
	*/
	uint8_t connected();
	
	/**
	* Write a single byte of data to the stream
	* @note This will write an entire TCP payload with only 1 byte in it
	*/
    size_t write(uint8_t);
	
	/**
	* Write a buffer of data, to be sent in a single TCP packet
	*/
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
    
	/**
	* Read a byte from the incoming buffer without advancing the point of reading
	*/
	int peek();
	
	/**
	* Flush all incoming client data from the current connection/buffer
	*/
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


#endif