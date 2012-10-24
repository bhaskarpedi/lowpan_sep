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
 *         A MAC protocol that does something!!! not any more-(not do anything)
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "nullmac.h"
#include "packetbuf.h"
#include "netstack.h"
#include "frame802154.h"
#include "bsp.h"

mac_state_t mac_state;
extern uint8_t mac_dsn;
extern uint16_t mac_dst_pan_id;

uint8_t mac_beacon_flag;  /* Indicates if a beacon should be tranmsitted */

/* Forward declaration*/
static void send_packet(mac_callback_t sent, void *ptr);
static void mac_start_timer(uint16_t beacon_interval_msec);
static void mac_stop_timer(void);
#ifdef LOWPAN_COORDINATOR
/* Function to send beacon */
extern void mac_send_beacon()
{
   static uint16_t macBSN = 0x00;
   uint8_t payload[10] = {0,0,0,0,0,0,0,0,0,0};
   frame802154_t params;
   static frame802154_beacon_t beaconData;
   uint8_t len;

   /* init to zeros */
   memset(&params, 0, sizeof(params));
   memset(&beaconData, 0, sizeof(frame802154_beacon_t));

   /* Build the FCF. */
   params.fcf.frame_type = FRAME802154_BEACONFRAME;
   params.fcf.security_enabled = 0;
   params.fcf.frame_pending = 0;
   params.fcf.ack_required = 0;
   params.fcf.panid_compression = 0;

   /* Insert IEEE 802.15.4 (2003) version bit. */
   params.fcf.frame_version = FRAME802154_IEEE802154_2003;

   /* Increment and set the beacon sequence number. */
   params.seq = macBSN++;

   /* Complete the addressing fields. */
   /**
     \todo For phase 1 the addresses are all long. We'll need a mechanism
     in the rime attributes to tell the mac to use long or short for phase 2.
    */
   params.fcf.src_addr_mode = FRAME802154_LONGADDRMODE;
   /* Set the source PAN ID to the PAN ID as per spec. */
   params.src_pid = mac_dst_pan_id;

   params.fcf.dest_addr_mode = FRAME802154_NOADDR;
   params.dest_pid = 0;
   //    params.dest_addr[0] = 0xFF;
   // params.dest_addr[1] = 0xFF;

   //rimeaddr_copy((rimeaddr_t *)&params.dest_addr,\
   packetbuf_addr(PACKETBUF_ADDR_RECEIVER));


   /*
    * Set up the source address using only the long address mode for
    * phase 1.
    */
   rimeaddr_copy((rimeaddr_t *)&params.src_addr, &rimeaddr_node_addr);
   rimeaddr_copy((rimeaddr_t *)&params.dest_addr, &rimeaddr_null);

   /* Preparing the payload for beacon*/
   beaconData.superFrSpec |= 0x000F;  /* BO set to 15 */
   beaconData.superFrSpec |= 0x00F0;  /* SO set to 15 */
   beaconData.superFrSpec |= 0x0F00;  /* CAP lt:No relevance in beacon less */ 
   beaconData.superFrSpec |= BV(14);  /* PAN coordinator */
   beaconData.superFrSpec |= BV(15);  /* Association permitted */

   beaconData.gtsInfo.gtsSpec = 0;    /* If 0, direction and list are absent*/

   beaconData.pendAddrInfo.pendAddrSpec = 0; /* If 0, no list present*/

   packetbuf_clear();
   len = frame802154_packBeacon((uint8_t*)packetbuf_dataptr(),&beaconData);
   packetbuf_set_datalen(len);
   params.payload = packetbuf_dataptr();
   len = frame802154_hdrlen(&params);
   frame802154_create(&params, packetbuf_hdrptr(), len);
   NETSTACK_RADIO.send(packetbuf_hdrptr(), packetbuf_totlen());
   return;
}

/* Function to process received assoc req*/
static int mac_proc_assoc_req(frame802154_t *frame)
{
   /* We implement the case when nothing is to be done for association */
   if(frame->payload[1] == 0x00)
   {
      return(0);
   }
   else
   {
      /* TODO: 16 bit short addr might be allocated here */
      return(-1);
   }
}

/* Function to send assoc rsp*/
static void mac_send_assoc_rsp(rimeaddr_t * nodeAddr)
{
   uint8_t * rime_ptr;
   uint8_t assoc_rsp_pkt[4];
   // 4 is the size of assoc rsp pkt
   // not using macro to save memory 

   /* Actual Assoc_Rsp packet */
   assoc_rsp_pkt[0] = MAC_ASSOC_RSP; 
  
   /* Address to indicate successful association without short
    * address allocation */
   assoc_rsp_pkt[1] = 0xFF;
   assoc_rsp_pkt[2] = 0xFE;

   /* 0x00 indicates successful transmission */
   assoc_rsp_pkt[3] = 0x00; 

   /* Now put that into packet buf format and send */
   packetbuf_copyfrom((const void *)&assoc_rsp_pkt[0], 4);

   /*Set the attributes and addresses*/
   packetbuf_set_attr(PACKETBUF_ATTR_RELIABLE,1);
   packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, nodeAddr);

   /* Copy payload into rime_ptr */

   send_packet((mac_callback_t)0,(void *)0);
}

#else /* Not Coordinator, regular node */

/* Function to process received beacon */
void mac_proc_beacon(frame802154_t *frame)
{
   //Do nothing for now. Just copy the src address to rimeaddr_panCoord_addr
   rimeaddr_copy(&rimeaddr_panCoord_addr, &frame->src_addr);
}

/* Function to send assoc_req */
static int mac_send_assoc_req()
{
   uint8_t * rime_ptr;
   uint8_t assoc_req_pkt[2];
   // 2 is the size of assoc req pkt
   // not using macro to save memory 

   /* Actual Assoc_REQ packet */
   assoc_req_pkt[0] = MAC_ASSOC_REQ; 
   assoc_req_pkt[1] = 0x00; /* All bits are zero in capabilities field byte */

   /* Now put that into packet buf format and send */
   packetbuf_copyfrom((const void *)&assoc_req_pkt[0], 2);

   /*Set the attributes and addresses*/
   packetbuf_set_attr(PACKETBUF_ATTR_RELIABLE,1);
   packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &rimeaddr_panCoord_addr);

   /* Copy payload into rime_ptr */

   send_packet((mac_callback_t)0,(void *)0);
}

/* Function to process assoc_rsp */
/* Assoc Rsp structure:
 * 1 byte - type
 * 2 bytes - short address
 * 1 byte - association status
 * (0XFFFF, 0XFFFE in short address for failed and no short address
 * cases respectively)  */

static int mac_proc_assoc_rsp(frame802154_t *frame)
{
   /* Only checking assoc status as short address is not implemented */
   if(frame->payload[3] == 0x00)
   {
      /*Association successful */
      return(0);
   }
   else
   {
      return(-1);
   }
}

#endif

#ifdef LOWPAN_COORDINATOR
/* State machine for co-ordinator is not relevant
 * The coordinator replies all requests */
static void mac_proc_pkt(frame802154_t *frame)
{
   if((frame->fcf.frame_type == MAC_CMD) && 
         (frame->payload[0] == MAC_ASSOC_REQ))
   {
      bspIState_t x;
      BSP_ENTER_CRITICAL_SECTION(x);
      mac_send_assoc_rsp(&frame->dest_addr);
      BSP_EXIT_CRITICAL_SECTION(x);
   }
   else if(frame->fcf.frame_type == MAC_DATA)
   {
      //route it appropriately
   }
}
#else
/* State machine for regular node */

#define MAX_HEAL_CNT 5
/*---------------------------------------------------------------------------*/
/* Passing stack pointers around functions is illegal. However as it is
 * a single threaded program, continuing to use it. */
static void mac_proc_state(frame802154_t *frame)
{
   static uint8_t healCnt = 0;
   healCnt ++;
   switch(mac_state)
   {
      case MAC_INIT:
         // Process only beacons 
         healCnt = 0;
         if(frame->fcf.frame_type == MAC_BEACON)
         {
            mac_proc_beacon(frame);
            mac_state = MAC_BEACON_RCVD;
            /* Using fall through to send ASSOC_REQ */
         }
         else
         {
            break;
         }

      case MAC_BEACON_RCVD:
         /* Prepare a assoc request packet and send it */
         mac_send_assoc_req();
         mac_state = MAC_ASSOC_REQ_SENT;
         break;

      case MAC_ASSOC_REQ_SENT:
         /* Process only Assoc rsp. Also check healCnt to restart SM */
         if((frame->fcf.frame_type == MAC_CMD) && 
               (frame->payload[0] == MAC_ASSOC_RSP))
         {
            int ret;
            ret = mac_proc_assoc_rsp(frame);
            if(0 == ret) /* success */
            {
               mac_state = MAC_CONNECTED;
               //TODO: Inform upper layer that MAC is ready
            }
            else
            {
               /* ASSOC failed, restart from next beacon */
               mac_state = MAC_INIT;
            }
         }
         else if(healCnt > MAX_HEAL_CNT)
         {
            mac_state = MAC_INIT;
         }
         break;
      
      case MAC_CONNECTED:
         // Ignore all other MAC Frames once MAC is connected 
         // TODO (later): Recovery method should be devised
         if(frame->fcf.frame_type == MAC_DATA)
         {
            NETSTACK_NETWORK.input();
         }
         break;

      default:
         /* Do nothing. Dropping packet */
         ;
   }
}
#endif


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
   //uint8_t * rime_ptr = NULL;
   frame802154_t frame;
   // TODO: BSKR is this buflen and not len??
   frame802154_parse(packetbuf_dataptr(), packetbuf_datalen(), &frame);
#ifdef LOWPAN_COORDINATOR
   mac_proc_pkt(&frame);
#else
   mac_proc_state(&frame);
#endif
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

/*---------------------------------------------------------------------------*/
static void
init(void)
{
   int retVal = MAC_TX_ERR;
   mac_state = MAC_INIT;
#ifdef LOWPAN_COORDINATOR
   /* Start Beacon Timer
    * Note: input is dummy for now*/
   mac_start_timer(10);
#endif

#if RIMEADDR_SIZE == 2
   rimeaddr_t addr_temp = { { 1, 2 } };
#elif (RIMEADDR_SIZE == 8)
   rimeaddr_t addr_temp = { { 0x12, 0x23, 0x34, 0x00, 0x00, 0x45, 0x00, 0x01 } };
#endif
   // Assign the node address from above address
   rimeaddr_copy(&rimeaddr_node_addr, &addr_temp);

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

#ifdef LOWPAN_COORDINATOR

/* Stop the beacon timer */
static void mac_stop_timer()
{
   /* Set Timer 1 mode to stop */
   TA1CTL |= MC_0;
}

/* Start the beacon timer */
static void mac_start_timer(uint16_t beacon_interval_msec)
{
   /* Set clock divider as 8 */
   TA1CTL |= ID_3;
   
   TA1EX0 |= 0x7;

   /* Clearing Timer A1 to ensure proper functioning of divide logic */
   TA1CTL |= TACLR;
   
   TA1R = 0; /* initial count  */

   /* Capture/ Control mode configuration */
   TA1CCTL0 |= CM1; /* Caputure mode rising edge */
   TA1CCTL0 |= CAP; /* Capture mode instead of compare mode */
   TA1CCTL0 |= CCIE; /* Enabling interrupts */

   /* compare count. (delay in ticks) */
   //TODO: TA1CCR0 = BSP_TIMER_CLK_MHZ*beacon_interval_msec;

   /* Start the timer in continous UP mode */
   /* It goes from 0 to 0xFFFF */
   TA1CTL |= MC_2;
   /* Enable interrupts */
   TA1CTL |= TAIE;
}

/**************************************************************************************************
 * @fn          mac_timer
 *
 * @brief       mac timer
 *
 * @param       -
 *
 * @return      -
 **************************************************************************************************
 */
BSP_ISR_FUNCTION(mac_timer_ISR, TIMER0_A1_VECTOR)
{
   /* Do I even need to have a comparision? As I am using only one CCR */
   if(TA1IV == TA1IV_TA1IFG)
   {
      mac_send_beacon();
   }

   TA1IV &= ~TA1IV_TA1IFG;
}


#endif
/*---------------------------------------------------------------------------*/
#if 0 /*scrap code for reference */
   frame802154_t params;
   uint8_t len;

   /* init to zeros */
   memset(&params, 0, sizeof(params));

   /* Build the FCF. */
   params.fcf.frame_type = FRAME802154_CMDFRAME;
   params.fcf.security_enabled = 0;
   params.fcf.frame_pending = 0;
   params.fcf.ack_required = 1; //BSKR: Dummy ACK
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
   
   params.src_pid = mac_src_pan_id;

   params.fcf.dest_addr_mode = FRAME802154_LONGADDRMODE;
   params.dest_pid = mac_dst_pan_id;

   /*
    * Set up the source address using only the long address mode for
    * phase 1.
    */
   rimeaddr_copy((rimeaddr_t *)&params.src_addr, &rimeaddr_node_addr);
   rimeaddr_copy((rimeaddr_t *)&params.dest_addr, &rimeaddr_panCoord_addr);

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

      NETSTACK_RADIO.send(&payload[8-len], len+2);
      return(0); // success
   } else {
      //PRINTF("6MAC-UT: too large header: %u\n", len);
   }
   return(1); // failure 
#endif
