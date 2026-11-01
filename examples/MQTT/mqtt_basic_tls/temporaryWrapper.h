/*
 * temporaryWrapper.h - TEMPORARY workaround for SSLClient + RF24Ethernet
 *
 *   An issue has been raised at https://github.com/govorox/SSLClient/issues/120
 *
 * Problem:
 *   SSLClient (govorox) reads from the underlying Client in its mbedTLS
 *   receive callback (client_net_recv_timeout() in ssl__client.cpp):
 *
 *       int result = client->read(buf, len);
 *       if (!result) return MBEDTLS_ERR_SSL_WANT_READ;
 *       return result;
 *
 *   Only a return value of 0 is treated as "no data yet". RF24Client::read()
 *   returns -1 when the connection is open but no data has arrived, which
 *   mbedTLS then sees as a fatal receive error. That breaks the handshake
 *   and the TLS session whenever data arrives more slowly than it's read,
 *   which is common over the RF24 link, especially with
 *   sslClient.setTimeout(0).
 *
 * Fix:
 *   This class wraps an EthernetClient and passes every call through.
 *   The one change: read(buf, size) returns 0 instead of -1 while the
 *   connection is still open. A real disconnect still returns -1, so
 *   errors still reach SSLClient.
 *
 * Usage:
 *   EthernetClient ethClient;
 *   TemporaryClientWrapper tlsTransport(ethClient);
 *   SSLClient sslClient(&tlsTransport);
 *
 * Remove this file and pass &ethClient straight to SSLClient once SSLClient
 * handles read() == -1 on an open connection as MBEDTLS_ERR_SSL_WANT_READ.
 */
#ifndef TEMPORARY_WRAPPER_H
#define TEMPORARY_WRAPPER_H

#include <Client.h>

class TemporaryClientWrapper : public Client {
public:
  explicit TemporaryClientWrapper(EthernetClient &client)
    : _client(client) {}

  int connect(IPAddress ip, uint16_t port) override {
    return _client.connect(ip, port);
  }
  int connect(const char *host, uint16_t port) override {
    return _client.connect(host, port);
  }

  size_t write(uint8_t b) override {
    return _client.write(b);
  }
  size_t write(const uint8_t *buf, size_t size) override {
    return _client.write(buf, size);
  }

  // available() also keeps the RF24/uIP stack running, so always pass it through
  int available() override {
    return _client.available();
  }

  int read() override {
    return _client.read();
  }

  // The fix: "no data yet" on an open connection returns 0 rather than -1
  int read(uint8_t *buf, size_t size) override {
    int result = _client.read(buf, size);
    if (result < 0 && _client.connected()) {
      return 0;
    }
    return result;
  }

  int peek() override {
    return _client.peek();
  }
  void flush() override {
    _client.flush();
  }
  void stop() override {
    _client.stop();
  }
  uint8_t connected() override {
    return _client.connected();
  }
  operator bool() override {
    return (bool)_client;
  }

private:
  EthernetClient &_client;
};

#endif  // TEMPORARY_WRAPPER_H
