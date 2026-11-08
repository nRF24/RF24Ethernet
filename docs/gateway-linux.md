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

## Install RF24* libraries and RF24Gateway (Raspberry Pi)

Download and run the installer script (builds/installs the required RF24* libraries):

```bash
wget https://raw.githubusercontent.com/nRF24/.github/main/installer/install.sh
chmod +x install.sh
./install.sh
```

Choose the **SPIDEV** driver. Near the end, the installer asks whether to install RF24Gateway as a service. There are two ways to run the gateway:

| | Option 1: systemd service (recommended) | Option 2: run an example manually |
|---|---|---|
| Starts at boot | Yes, restarts itself if it exits | No |
| Runs as | Unprivileged user (`rf24gateway`) with only `CAP_NET_ADMIN` | Your user, usually with `sudo` |
| Settings | `/etc/rf24gateway/rf24gateway.conf`, no rebuild | Edit the example source and rebuild |
| Best for | A permanent gateway | Quick tests and development |

> [!important]
> Only one program can use the radio. Stop the service (`sudo systemctl stop rf24gateway`) before running an example manually, and vice versa.

### Option 1: Run RF24Gateway as a service (recommended)

Answer **y** to *"Do you want to install RF24Gateway as a service that starts at boot"* and choose a mode:

1. **Headless**: no interface, output goes to the system log (journald)
2. **GUI-noInterrupts**: the ncurses interface in a `screen` session, polling the radio (default)
3. **GUI-Interrupts**: the ncurses interface, using the radio's IRQ pin

The installer then asks for the service user, the `tun_nrf24` IP address and netmask, the radio channel, an optional list of allowed nodeIDs, and (if the libraries were built with CCM) an AES-CCM encryption key. It builds `rf24gatewayd`, installs and enables `rf24gateway.service`, and offers to start it.

What gets installed:

| Path | Purpose |
|---|---|
| `/etc/rf24gateway/rf24gateway.conf` | Settings (CE/CSN/IRQ pins, node ID, channel, data rate, IP, `allowed_nodes`, encryption). Kept on reinstall |
| `/etc/rf24gateway/rf24gateway.key` | AES-CCM key, if encryption is enabled (mode 640, `root:rf24gateway`) |
| `/var/lib/rf24gateway/` | `dhcplist.txt`, `routing.txt`, `failLog.txt` |
| `/usr/local/bin/rf24gatewayd` | The gateway program |
| `/usr/local/bin/rf24gateway-ui` | Attaches to the ncurses UI (GUI modes only) |
| `/etc/systemd/system/rf24gateway.service` | The systemd unit |

Managing the service:

```bash
sudo systemctl status rf24gateway     # is it running?
sudo systemctl restart rf24gateway    # apply changes to rf24gateway.conf
sudo systemctl stop rf24gateway       # free the radio, e.g. to run an example
sudo systemctl disable rf24gateway    # don't start at boot
journalctl -u rf24gateway -f          # log output (headless mode, startup errors, released nodes)
rf24gateway-ui                        # GUI modes: view the UI. Detach with Ctrl-A then D
```

> [!note]
> In the GUI modes, pressing `x` in the UI exits the program, but systemd restarts it after 5 seconds. Use `systemctl stop` to stop it.

To build the service without the installer (the libraries must already be installed):

```bash
cd rf24libs/RF24Gateway/service
mkdir build && cd build
cmake .. -DRF24GATEWAY_SERVICE_MODE=ncurses   # or headless / ncursesint; add -DRF24NETWORK_CCM=ON for encryption
make
sudo make install
sudo nano /etc/rf24gateway/rf24gateway.conf
sudo systemctl daemon-reload
sudo systemctl enable --now rf24gateway
```

`-DRF24GATEWAY_USER=<name>` changes the service user. The GUI modes need `libncurses-dev` and `screen`. The pigpio driver needs root, so the service requires SPIDEV.

### Option 2: Run the ncurses example manually

```bash
cd rf24libs/RF24Gateway/examples/build
make
cd ncurses
./RF24Gateway_ncurses
```

The examples keep `dhcplist.txt` in the current directory, separate from the service's `/var/lib/rf24gateway/dhcplist.txt`, so nodes may be given different addresses when switching between the two.

---

## Quick checklist (sanity)

Before configuring routing, make sure:

1. The radio is detected and can communicate (gateway UI/logs show packets / node activity; for the service, `sudo systemctl status rf24gateway` and `journalctl -u rf24gateway`)
2. The TUN/TAP interface exists and is **UP**
3. Linux has a route for the RF24 subnet via that interface

---

## Step 1: Create/enable a TUN interface

The **service creates `tun_nrf24` and sets its address automatically** from `ip` and `netmask` in `/etc/rf24gateway/rf24gateway.conf` (it has `CAP_NET_ADMIN` for this), so skip to the verify commands below.

When running an example, RF24Gateway creates the interface automatically if run as `root`. If running it unprivileged, create the TUN interface manually:

```bash
# Create a persistent TUN device named tun_nrf24, owned by the current user
sudo ip tuntap add dev tun_nrf24 mode tun user $USER multi_queue

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
- RF24Gateway output/logs (do nodes join mesh / show activity?): `rf24gateway-ui` or `journalctl -u rf24gateway -f`
- Service: `channel` and `data_rate` in `/etc/rf24gateway/rf24gateway.conf` match the nodes
- Service: if `allowed_nodes` is set, the node's ID is listed. Unlisted nodes are released and logged in `journalctl -u rf24gateway` (see RF24Gateway's `docs/securing_the_network.md`)
- Service with `encryption = 1`: every node must use the same key, and nRF24L01 nodes without encryption can't join

### The service won't start or keeps restarting
- `journalctl -u rf24gateway -b` shows the reason (an invalid config line, a missing key file, radio not found)
- Another gateway program is using the radio: stop it (`pgrep -af RF24Gateway`)
- The service user must be in the `spi` and `gpio` groups: `id rf24gateway`. Fix with `sudo usermod -aG spi,gpio rf24gateway`
- `encryption = 1` but the service was built without CCM: rebuild with `-DRF24NETWORK_CCM=ON` or set `encryption = 0`

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

- Keep RF24 subnets small and simple at first (one /24). The service defaults (`10.10.2.2`, `255.255.255.0`) match the examples in this guide.
- Once you have stable ping, then add TCP services (HTTP/MQTT) to reduce variables.
- If you have performance issues, prefer lwIP on faster MCUs (configured via `RF24BoardConfig.h`).