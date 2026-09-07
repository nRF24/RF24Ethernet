

extern "C"{

struct net_if *rf24_netif_get_iface(void);
int rf24_cpp_tx_frame(const uint8_t *data, size_t len);
int rf24_netif_deliver_frame(const uint8_t *data, size_t len);


}