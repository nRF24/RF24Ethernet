/*
 * secrets.h - private settings for the mqtt_basic_tls example
 *
 * Fill in your own values below. Do not commit real credentials,
 * certificates or keys to a public repository.
 */
#ifndef SECRETS_H
#define SECRETS_H

/**** WiFi credentials ****/
#define SECRET_SSID "your-ssid"
#define SECRET_PASS "your-wifi-password"

/**** MQTT broker credentials ****/
// Comment out SECRET_MQTT_USER to connect without a username/password
#define SECRET_MQTT_USER "username"
#define SECRET_MQTT_PASS "password"

/**** TLS certificates ****/
// CA certificate that signed the broker's certificate: output of
//   cat /etc/mosquitto/certs/ca.crt
// The broker cert's SAN must contain the IP we connect to (e.g. IP:10.10.2.2).
const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...paste your CA certificate here...
-----END CERTIFICATE-----
)EOF";

// Only used when TLS_CLIENT_CERT is defined in the sketch
// (mosquitto: require_certificate true)
const char client_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...paste your client certificate here...
-----END CERTIFICATE-----
)EOF";

const char client_key[] PROGMEM = R"EOF(
-----BEGIN EC PRIVATE KEY-----
...paste your client private key here...
-----END EC PRIVATE KEY-----
)EOF";

#endif  // SECRETS_H
