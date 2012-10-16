/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: nullmac.c,v 1.15 2010/06/14 19:19:17 adamdunkels Exp $
 */

/**
 * \file
 *         A MAC protocol that does not do anything.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "nullmac.h"
#include "packetbuf.h"
#include "netstack.h"

typedef enum
{
	MAC_INIT = 0,
	MAC_SCAN,
   MAC_ASSOC_REQ_SENT,
   MAC_CONNECTED,
	MAC_INVALID
}mac_state_t;

// This structure is kind of also defined in frame802154.h	
typedef enum
{
	MAC_BEACON =		0,
	MAC_DATA = 			1,
	MAC_ACK = 			2,
	MAC_CMD = 			3,
	MAC_RANGING =		14,
	MAC_TYPE_NONE =	15
}mac_frame_type_t;

typedef enum
{
   MAC_ASSOC_REQ = 0x01,
   MAC_ASSOC_RSP = 0x02,
   MAC_DISASSOC_NOT = 0x03,
   MAC_DATA_REQ = 0x04,
   MAC_PANID_CONFLICT = 0x05,
   MAC_ORPHAN_NOT = 0x06,
   MAC_BEACON_REQ = 0x07,
   MAC_COORD_REALIGN = 0x08,
   MAC_GTS_REQ = 0x09   
}mac_cmd_frame_t;

static mac_state_t mac_state;

/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
  NETSTACK_RDC.send(sent, ptr);
}
/*---------------------------------------------------------------------------*/
static void
packet_input(void)
{
  NETSTACK_NETWORK.input();
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
//#define DISPATCH_BYTE_POS 0
//#define 802154_CMDFRM_ID_POS 0
   static void
lowpan_packet_input(void)
{
   uint8_t * rime_ptr = NULL;
   frame802154_t frame;
   // TODO: BSKR is this buflen and not len??
   frame802154_parse(packetbuf_dataptr(), buflen, &frame); 
   if((frame.fcf.frame_type == MAC_DATA))
   {
      NETSTACK_NETWORK.input();
   }
   else if (frame.fcf.frame_type == MAC_CMD)
   {
      switch((uint8_t)frame.payload[0])
      {
         case MAC_ASSOC_REQ:
            break;

         case MAC_ASSOC_RSP:
            break;

         case MAC_DATA_REQ:
            break;

         default:
            // Other command frames not handled
      }
      /* Process Association */
   }
   else
   {
      /* Other 802.15.4 MAC frames are not handled here */
   }
}

static int
on(void)
{
  return NETSTACK_RDC.on();
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  return NETSTACK_RDC.off(keep_radio_on);
}
/*---------------------------------------------------------------------------*/
static unsigned short
channel_check_interval(void)
{
  return 0;
}

static int mac_send_assoc_req()
{
   int retVal = MAC_TX_ERR;
   uint8_t payload[10] = {0,0,0,0,0,0,0,0,0,0};
   frame802154_t params;
   uint8_t len;

   /* init to zeros */
   memset(&params, 0, sizeof(params));

   /* Build the FCF. */
   params.fcf.frame_type = FRAME802154_CMDFRAME;
   params.fcf.security_enabled = 0;
   params.fcf.frame_pending = 0;
   params.fcf.ack_required = 1;//BSKR: Not sure if ACK is implemented
   params.fcf.panid_compression = 0;

   /* Insert IEEE 802.15.4 (2003) version bit. */
   params.fcf.frame_version = FRAME802154_IEEE802154_2003;

   /* Increment and set the data sequence number. */
   params.seq = mac_dsn++;

   /* Complete the addressing fields. */
   /**
     \todo For phase 1 the addresses are all long. We'll need a mechanism
     in the rime attributes to tell the mac to use long or short for phase 2.
    */
   params.fcf.src_addr_mode = FRAME802154_LONGADDRMODE;
   /* Set the source PAN ID to the 0xFFFF as per spec. */
   params.src_pid = 0xFFFF;

   params.fcf.dest_addr_mode = FRAME802154_LONGADDRMODE;
   params.dest_pid = mac_dst_pan_id;
   //    params.dest_addr[0] = 0xFF;
   // params.dest_addr[1] = 0xFF;

   //rimeaddr_copy((rimeaddr_t *)&params.dest_addr,\
   packetbuf_addr(PACKETBUF_ADDR_RECEIVER));


   /*
    * Set up the source address using only the long address mode for
    * phase 1.
    */
   rimeaddr_copy((rimeaddr_t *)&params.src_addr, &rimeaddr_node_addr);

   /* Preparing the payload for ASSOC REQ */
   // Filling the last two bytes, reserving the earlier bytes for hdr
   payload[8] = MAC_ASSOC_REQ; 
   // Filling capability Info byte
   // From Bit 7 to 0:
   // PAN Coord, FFD, Pwr Src, Rx always on, Res, Sec, 16bit address
   payload[9] |= 0x0;  
   params.payload = &payload[8];
   params.payload_len = 2;
   len = frame802154_hdrlen(&params);
   if(len <= 8) {
      frame802154_create(&params, &payload[8-len], len);

      retVal = NETSTACK_RADIO.send(&payload[8-len], len+2);
   } else {
      PRINTF("6MAC-UT: too large header: %u\n", len);
   }
   return(retVal);
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
   int retVal = MAC_TX_ERR;
   mac_mode = MAC_INIT;
   // TODO: Assign MAC address to the node from config file
#if RIMEADDR_SIZE == 2
   rimeaddr_node_addr = { { 1, 2 } };
#else /*RIMEADDR_SIZE == 2*/
#if RIMEADDR_SIZE == 8
   rimeaddr_node_addr = 
         { { 0x12, 0x23, 0x34, 0x00, 0x00, 0x45, 0x00, 0x01 } };
#endif /*RIMEADDR_SIZE == 8*/
#endif /*RIMEADDR_SIZE == 2*/

#ifdef LOWPAN_COORDINATOR
   /* Do nothing here. Initialize data structures used for ASSOC reply */
#else
   /* Scanning is by passed. Coordinator details are configured */
   /* Send an ASSOC request */
   do{
      retVal = mac_send_assoc_req();
      mac_mode = MAC_ASSOC_REQ_SENT;
   }while(MAC_TX_OK != retVal)
#endif
}
/*---------------------------------------------------------------------------*/
const struct mac_driver nullmac_driver = {
  "nullmac",
  init,
  send_packet,
  packet_input,
  on,
  off,
  channel_check_interval,
};
/*---------------------------------------------------------------------------*/
const struct mac_driver nullmac_802154_driver = {
  "nullmac",
  init,
  send_packet,
  lowpan_packet_input,
  on,
  off,
  channel_check_interval,
};
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
