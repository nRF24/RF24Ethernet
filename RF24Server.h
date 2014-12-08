#ifndef RF24SERVER_H
#define RF24SERVER_H


#import "Server.h"
#import "RF24Client.h"
#include "ethernet_comp.h"

class RF24Server : public Server {

public:
  RF24Server(uint16_t);
  RF24Client available();
  void begin();
  size_t write(uint8_t);
  size_t write(const uint8_t *buf, size_t size);
  using Print::write;

private:
  uint16_t _port;
};

#endif