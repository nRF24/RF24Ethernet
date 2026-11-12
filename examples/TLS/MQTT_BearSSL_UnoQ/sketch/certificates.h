/*
 * certificates.h - TLS trust anchor for the broker's CA certificate
 *
 * BearSSL does not take a PEM CA certificate. It needs the CA converted to a
 * br_x509_trust_anchor in C. Generate it from the broker's CA with either:
 *   brssl ta /etc/mosquitto/certs/ca.crt
 *   (brssl is built with BearSSL: https://bearssl.org)
 * or OPEnSLab's tool:
 *   python3 pycert_bearssl.py convert --no-search /etc/mosquitto/certs/ca.crt
 *   (https://github.com/OPEnSLab-OSU/SSLClient/tree/master/tools/pycert_bearssl)
 *
 * Replace everything between the markers below with the output.
 * The array must be named TAs and the count TAs_NUM.
 */
#ifndef CERTIFICATES_H
#define CERTIFICATES_H

#include <ArduinoBearSSL.h>

// ---- BEGIN generated trust anchor ----
#error "Paste your generated trust anchor into certificates.h, then delete this line"
// static const unsigned char TA0_DN[] = { ... };
// static const unsigned char TA0_EC_Q[] = { ... };   (or TA0_RSA_N / TA0_RSA_E)
// static const br_x509_trust_anchor TAs[] = { ... };
// #define TAs_NUM 1
// ---- END generated trust anchor ----

#endif  // CERTIFICATES_H
