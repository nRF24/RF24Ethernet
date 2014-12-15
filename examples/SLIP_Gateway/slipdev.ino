/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup slip Serial Line IP (SLIP) protocol
 * @{
 *
 * The SLIP protocol is a very simple way to transmit IP packets over
 * a serial line. It does not provide any framing or error control,
 * and is therefore not very widely used today.
 *
 * This SLIP implementation requires two functions for accessing the
 * serial device: slipdev_char_poll() and slipdev_char_put(). These
 * must be implemented specifically for the system on which the SLIP
 * protocol is to be run.
 */

/**
 * \file
 * SLIP protocol implementation
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Update Dec 2014 - TMRh20
 * - Modified for RF24Ethernet/RF24Network
 *
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 *
 */

/*
 * This is a generic implementation of the SLIP protocol over an RS232
 * (serial) device. 
 *
 * Huge thanks to Ullrich von Bassewitz <uz@cc65.org> of cc65 fame for
 * and endless supply of bugfixes, insightsful comments and
 * suggestions, and improvements to this code!
 */

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335


static uint16_t len, tmplen;
static uint8_t lastc;
HardwareSerial *slip_device;

/*-----------------------------------------------------------------------------------*/

// Put a character on the serial device.
void slipdev_char_put(uint8_t c)
{
	slip_device->write((char)c);
}

/*-----------------------------------------------------------------------------------*/

// Poll the serial device for a character.
uint8_t slipdev_char_poll(uint8_t *c)
{
	if (slip_device->available()) {
		*c = slip_device->read();
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------------------*/
/**
 * Send the packet in the uip_buf and uip_appdata buffers using the
 * SLIP protocol.
 *
 * The first 40 bytes of the packet (the IP and TCP headers) are read
 * from the uip_buf buffer, and the following bytes (the application
 * data) are read from the uip_appdata buffer.
 *
 * TMRh20: This function now takes in a pointer to an array, and the length of the data
 *
 */
/*-----------------------------------------------------------------------------------*/
void slipdev_send(uint8_t *ptr, size_t len)
{
  uint16_t i;
  uint8_t c;

  slipdev_char_put(SLIP_END);
  
  #if defined (LED_TXRX)
  digitalWrite(DEBUG_LED_PIN,HIGH);
  #endif
  
  for(i = 0; i < len; ++i) {
    c = *ptr++;
    switch(c) {
    case SLIP_END:
      slipdev_char_put(SLIP_ESC);
      slipdev_char_put(SLIP_ESC_END);
      break;
    case SLIP_ESC:
      slipdev_char_put(SLIP_ESC);
      slipdev_char_put(SLIP_ESC_ESC);
      break;
    default:
      slipdev_char_put(c);
      break;
    }
  }
  slipdev_char_put(SLIP_END);
  #if defined (LED_TXRX)
  digitalWrite(DEBUG_LED_PIN,LOW);
  #endif
}
/*-----------------------------------------------------------------------------------*/
/** 
 * Poll the SLIP device for an available packet.
 *
 * This function will poll the SLIP device to see if a packet is
 * available. It uses a buffer in which all avaliable bytes from the
 * RS232 interface are read into. When a full packet has been read
 * into the buffer, the packet is copied into the uip_buf buffer and
 * the length of the packet is returned.
 *
 * \return The length of the packet placed in the uip_buf buffer, or
 * zero if no packet is available.
 */
/*-----------------------------------------------------------------------------------*/
uint16_t slipdev_poll(void)
{
  uint8_t c;


 // Create a new RF24Network header if there is data available, and possibly ready to send
 if(slip_device->available()){  
  
  while((uint8_t)slipdev_char_poll(&c)) {
    switch(c) {
    case SLIP_ESC:
      lastc = c;
      break;

    case SLIP_END:
      lastc = c;
      /* End marker found, we copy our input buffer to the uip_buf
	 buffer and return the size of the packet we copied. */

      // Ensure the data is no longer than the configured UIP buffer size
      len = min(len,UIP_BUFFER_SIZE);
      
      tmplen = len;
      len = 0;
      return tmplen;

    default:
      if(lastc == SLIP_ESC) {
	lastc = c;
	/* Previous read byte was an escape byte, so this byte will be
	   interpreted differently from others. */
	switch(c) {
	case SLIP_ESC_END:
	  c = SLIP_END;
	  break;
	case SLIP_ESC_ESC:
	  c = SLIP_ESC;
	  break;
	}
      } else {
	lastc = c;
      }

      slip_buf[len] = c;
      ++len;

      if(len > UIP_BUFFER_SIZE) {
	len = 0;
      }

      break;
    }
  }
 }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the SLIP module.
 *
 * This function does not initialize the underlying RS232 device, but
 * only the SLIP part.
 */ 
/*-----------------------------------------------------------------------------------*/
void slipdev_init(HardwareSerial &dev){
  lastc = len = 0;
  slip_device = &dev;
}
/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */

