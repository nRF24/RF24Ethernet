# Configuration and Set-Up

@tableofcontents

RF24Ethernet requires the RF24 and RF24Network libraries (optionally RF24Mesh).

See <http://tmrh20.github.io> for documentation and downloads

See [this video walk-through](https://www.youtube.com/watch?v=eYiWUTjNSuc) of the software setup with Raspberry Pi and Arduino.

## Linux Gateway (Raspberry Pi, etc)

Gateway setup is documented here:

- [Linux Gateway (RF24Gateway + TUN/TAP)](gateway-linux.md)

## Arduino

1. Install **RF24Ethernet** from the Arduino Library Manager. This should automatically install **RF24**, **RF24Network** and **RF24Mesh**.
2. For most devices with processors **≥ 50MHz**, the **Arduino lwIP** library needs to be installed from the Arduino Library Manager for proper performance and stability.  
   **ESP32/ESP8266-based boards already use lwIP for WiFi**, so the separate **lwIP Arduino library is not required** on those platforms & similar.
3. **Required:** Configure your radio **CE/CSN pin assignments** to match your wiring/board/shield.  
   **Optional:** Adjust other RF24/RF24Network/RF24Mesh settings as needed (channel, data rate, nodeID, etc.).

#### 4a. Running with a Gateway

4. Load one of the included examples:
   - `RF24Ethernet/examples/Getting_Started_SimpleClient/Getting_Started_SimpleClient.ino`
   - `RF24Ethernet/examples/Getting_Started_SimpleServer/Getting_Started_SimpleServer.ino`
5. Configure the node’s IP address and set the **last octet** to match its **nodeID**. For example, nodeID `3` should be assigned `10.10.2.3`.  
   - Set the **gateway** to the chosen IP of the RPi running RF24Gateway (for example `10.10.2.2`).
6. Upload the example sketch to your Arduino and verify connectivity:
   - With `SimpleServer`, browse to `http://10.10.2.3:1000` (adjust IP/nodeID as needed).
   - With `SimpleClient`, configure the server IP as needed and observe Serial output.

#### 4b. Running headless (no Gateway / no RPi)

4. If you are **not** running a gateway (for example, no RPi running RF24Gateway), use the headless examples found under `/examples/Headless/`:
   - `RF24Ethernet/examples/Headless/InteractiveServer_Mesh_Headless/InteractiveServer_Mesh_Headless.ino`
   - `RF24Ethernet/examples/Headless/SimpleClient_Mesh_Headless/SimpleClient_Mesh_Headless.ino`
5. Upload the sketches and follow the prompts/output in the Serial Monitor to interact with the nodes (no gateway required).

**Memory-constrained Arduinos:**  
Uncomment `#define DISABLE_USER_PAYLOADS` in `RF24Network_config.h` to save memory (recommended for RF24Ethernet, but not for typical RF24Network “user payload” use).

## Non-Raspberry Pi (Linux etc) Devices

Arduino can also function as a gateway for any Linux machine or PC/MAC that supports SLIP.

See the SLIP_Gateway and SLIP_InteractiveServer
examples for usage without the need for a Raspberry Pi.

## Advanced (uIP) Configuration and Info

See the [User Configuration](group__UipConfiguration.html) section for modifying uip-conf.h
