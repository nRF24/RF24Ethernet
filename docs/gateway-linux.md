# Linux Gateway (RF24Gateway + TUN/TAP)

This project is typically used with a **Linux gateway** (often a Raspberry Pi) to bridge the RF24 mesh/network to a normal IP network using a **TUN/TAP** interface.

If you only want node-to-node demos, start with the `Headless` examples first. If you want to reach services on your LAN (or run services on RF24 nodes and access them from LAN clients), set up the gateway.

---

## What the gateway does

- Runs an RF24 radio + networking stack on Linux
- Creates a virtual network interface (**TUN/TAP**) that looks like a normal NIC to Linux
- Routes IP traffic between:
  - **RF24 nodes** (RF24Ethernet)
  - your **LAN** (eth0/wlan0)

---

## Prerequisites

- A Linux machine (Raspberry Pi recommended)
- A compatible RF24 radio connected to the Linux machine (SPI wiring + stable power)
- This repo’s Arduino sketches running on nodes (RF24 + RF24Network + RF24Mesh + RF24Ethernet)

> [!note]
> The gateway software is provided by [nRF24/RF24Gateway](https://github.com/nRF24/RF24Gateway).
> Docs: <https://nrf24.github.io/RF24Gateway/>

---

## Install RF24* libraries and build RF24Gateway (Raspberry Pi)

1. Download and run the installer script (builds/installs the required RF24* libraries):

   ```bash
   wget https://raw.githubusercontent.com/nRF24/.github/main/installer/install.sh
   chmod +x install.sh
   ./install.sh
   ```

2. Build and run the RF24Gateway ncurses example:

   ```bash
   cd rf24libs/RF24Gateway/examples/build
   make
   cd ncurses
   ./RF24Gateway_ncurses
   ```

---

## Quick checklist (sanity)

Before configuring routing, make sure:

1. The radio is detected and can communicate (gateway logs show packets / node activity)
2. The TUN/TAP interface exists and is **UP**
3. Linux has a route for the RF24 subnet via that interface

---

## Step 1: Create/enable a TUN interface

RF24Gateway can create the interface automatically when running as `root`. If running unprivileged, you may need to create the TUN interface manually:

```bash
# Create a persistent TUN device named tun_nrf24, owned by user "pi"
sudo ip tuntap add dev tun_nrf24 mode tun user pi multi_queue

# Assign the gateway-side IP for the RF24 subnet
sudo ip addr add 10.10.2.2/24 dev tun_nrf24

# Bring it up
sudo ip link set tun_nrf24 up
```

Verify:

```bash
ip addr show tun_nrf24
ip link show tun_nrf24
ip route | grep 10.10.2.0/24
```

---

## Step 2: Enable IP forwarding

If you want traffic to route between interfaces (tun_nrf24 <-> eth0/wlan0):

```bash
sudo sysctl -w net.ipv4.ip_forward=1
```

To make it persistent, add to `/etc/sysctl.conf`:

```conf
net.ipv4.ip_forward=1
```

---

## Step 3: Add routes (LAN <-> RF24 subnet)

Usually Linux learns the connected route once you assign the `tun_nrf24` address, but you can confirm:

```bash
ip route
```

You should see something like:

- `10.10.2.0/24 dev tun_nrf24  proto kernel  scope link  src 10.10.2.2`

If your LAN clients need to reach RF24 nodes, you have two common options:

### Option A: Add a static route on your router (preferred)
Add a route on your LAN router:

- Destination: `10.10.2.0/24`
- Next hop: `<gateway-lan-ip>` (the Raspberry Pi’s IP on your LAN)

This avoids NAT and keeps the network transparent.

### Option B: NAT (masquerade) traffic from RF24 to LAN/Internet
This is easier when you cannot change the router.

Example (replace `wlan0` with your LAN-facing interface):

```bash
sudo iptables -t nat -A POSTROUTING -s 10.10.2.0/24 -o wlan0 -j MASQUERADE
```

If you want this to persist, use your distro’s preferred firewall persistence mechanism (e.g., `iptables-persistent`, `nftables`, or a systemd unit).

---

## Optional: TCP buffer tuning (throughput)

To maximize throughput between Raspberry Pi or other Linux devices, you can set TCP buffer sizes. To make these changes permanent, edit `/etc/sysctl.conf`.

```bash
sudo sysctl net.ipv4.tcp_wmem="1500 1500 1500"
sudo sysctl net.ipv4.tcp_rmem="1500 1500 1500"
```

> [!warning]
> These commands can severely impact other network functionality. Run `sudo sysctl net.ipv4.tcp_wmem` and `sudo sysctl net.ipv4.tcp_rmem` first to record defaults so they can be restored.

---

## Step 4: Test connectivity

### From the gateway
Ping a node IP:

```bash
ping 10.10.2.3
```

### From a LAN machine (if routing is configured)
Ping the same node:

```bash
ping 10.10.2.3
```

### Watch traffic on the tunnel
On the gateway:

```bash
sudo tcpdump -ni tun_nrf24
```

You should see ICMP/TCP/UDP packets when nodes communicate.

---

## Common problems

### TUN interface is up but nothing responds
- Wrong subnet or IP mismatch between sketches and gateway
- RF24Gateway is running but not attached to the same RF24 channel/addressing as nodes
- RF24 radio power problems (brownouts) or SPI wiring issues

**Checks**
- `tcpdump -ni tun_nrf24` (do packets appear at all?)
- RF24Gateway output/logs (do nodes join mesh / show activity?)

### Nodes can talk to gateway but LAN cannot reach nodes
- Missing route on router (Option A) or missing NAT rule (Option B)
- `net.ipv4.ip_forward=0`
- Firewall blocking forwarding

**Checks**
- `sysctl net.ipv4.ip_forward`
- `ip route`
- `sudo iptables -S` and `sudo iptables -t nat -S` (or `nft list ruleset`)

### DNS works on LAN but not on nodes
- Nodes need a reachable DNS server IP (often your router)
- **uIP:** UDP may need to be manually enabled on the node for DNS to work
- **lwIP:** UDP is enabled by default
- Gateway must route UDP/53 properly (and not block it)
- Try the `Getting_Started_SimpleClient_Mesh_DNS` example first

---

## Notes

- Keep RF24 subnets small and simple at first (one /24).
- Once you have stable ping, then add TCP services (HTTP/MQTT) to reduce variables.
- If you have performance issues, prefer lwIP on faster MCUs (configured via `RF24BoardConfig.h`).