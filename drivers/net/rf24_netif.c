
#if USE_LWIP == 2


#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/dummy.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <zephyr/toolchain.h>


#ifndef RF24_MTU
#define RF24_MTU 1500
#endif

/* Provided by your C++ bridge side */
extern int rf24_cpp_tx_frame(const uint8_t *data, size_t len);

static struct net_if *rf24_iface;

static uint8_t buf[RF24_MTU];
static K_MUTEX_DEFINE(send_mutex);
    
/* ---------------- TX: Zephyr -> lwIP/TUN bridge ---------------- */
static int rf24_vsend(const struct device *dev, struct net_pkt *pkt)
{
    ARG_UNUSED(dev);
    
    if(!pkt){
        printk("NO packet ****************\n");
        return -1;
    }
    
    size_t frame_len = net_pkt_get_len(pkt);
    if (frame_len == 0U || frame_len > (RF24_MTU)) {
        return -EMSGSIZE;
    }

    k_mutex_lock(&send_mutex, K_FOREVER);  // Lock before touching buf
    
    net_pkt_cursor_init(pkt);
    int rc = net_pkt_read(pkt, buf, frame_len);
    if (rc < 0) {
        k_mutex_unlock(&send_mutex);
        return rc;
    }

    rc = rf24_cpp_tx_frame(buf, frame_len);
    
    k_mutex_unlock(&send_mutex);  // Unlock after send
    
    return (rc < 0) ? rc : 0;
}

/* ---------------- iface init ---------------- */
static void rf24_iface_init(struct net_if *iface)
{
    #if defined(RF24ETHERNET_DEBUG_CLIENT)
        printk("rf24_iface_init iface=%p dev=%p\n", iface, net_if_get_device(iface));
    #endif
	rf24_iface = iface;

	/* Optional: set link addr if you use one; for dummy L2 often unnecessary */
	net_if_flag_set(iface, NET_IF_NO_AUTO_START);
	net_if_up(iface);
}

struct net_if *rf24_netif_get_iface(void)
{
	return rf24_iface;
}

/* ---------------- RX: lwIP/TUN bridge -> Zephyr ---------------- */
int rf24_netif_deliver_frame(const uint8_t *ip_pkt, size_t len)
{
	struct net_if *iface = rf24_netif_get_iface();
	struct net_pkt *pkt;
	int ret;

	if (!iface || !ip_pkt || len < 20U) {
		return -EINVAL;
	}

	uint8_t ihl = (ip_pkt[0] & 0x0FU) * 4U;
	if (ihl < 20U || ihl > len) {
		return -EINVAL;
	}

	uint16_t ip_len = ((uint16_t)ip_pkt[2] << 8) | ip_pkt[3];
	if (ip_len < ihl || ip_len > len || ip_len > (RF24_MTU + 64U)) {
		return -EINVAL;
	}

	enum net_ip_protocol proto = (enum net_ip_protocol)ip_pkt[9];

	pkt = net_pkt_rx_alloc_with_buffer(iface, ip_len, AF_INET, proto, K_NO_WAIT);
	if (!pkt) {
		return -ENOMEM;
	}

	net_pkt_set_iface(pkt, iface);
	net_pkt_set_family(pkt, AF_INET);
	net_pkt_set_ip_hdr_len(pkt, ihl);

	ret = net_pkt_write(pkt, ip_pkt, ip_len);
	if (ret < 0) {
		net_pkt_unref(pkt);
		return ret;
	}

	net_pkt_cursor_init(pkt);

	ret = net_recv_data(iface, pkt);
	if (ret < 0) {
        printk("Kernel rejected the frame: %d\n", ret);
		net_pkt_unref(pkt);
		return ret;
	}

	return 0;
}

/* ---------------- device api ---------------- */
static const struct dummy_api rf24_api = {
    .iface_api.init = rf24_iface_init,
    .send = rf24_vsend,
};

static int rf24_dev_init(const struct device *dev)
{
    ARG_UNUSED(dev);
    return 0;
}

NET_DEVICE_INIT(rf24_netif, "rf24_netif",
                rf24_dev_init, NULL, NULL, NULL,
                80,
                &rf24_api, DUMMY_L2, DUMMY_L2_CTX_TYPE, RF24_MTU);
                
                
#endif //USE_LWIP == 2