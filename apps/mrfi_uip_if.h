/*
 * Defines, common structures, externs between MRFI and the upper layers
 */


/* This flag is set by mrfi whenever a packet is copied to its
 * buffer and is ready to be processed.
 * The application polls for this flag periodically and processes
 * the received packets */
#include "stdint.h"
#include "mrfi.h"

//extern uint8_t mrfi_uip_ind_flag;

//extern void mrfi_uip_pkt_convert(void *pPacket, uint16_t pktLen);

extern void mrfi_uip_pkt_proc_done(void);
