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
 * $Id: nullmac.h,v 1.4 2010/02/18 21:48:39 adamdunkels Exp $
 */

/**
 * \file
 *         A MAC protocol implementation that does not do anything.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef __NULLMAC_H__
#define __NULLMAC_H__

#include "mac.h"
#include "radio.h"
#include "packetbuf.h"

#define BV(n)      (1 << (n))

typedef enum
{
	MAC_INIT = 0,
	MAC_BEACON_RCVD,
   MAC_ASSOC_REQ_SENT,
   MAC_CONNECTED
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

extern const struct mac_driver nullmac_driver;
extern uint8_t mac_beacon_flag;  /* Indicates if a beacon should be tranmsitted */

#ifdef LOWPAN_COORDINATOR
extern void mac_send_beacon();
#endif

#endif /* __NULLMAC_H__ */
