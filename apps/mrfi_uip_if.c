#include "mrfi_uip_if.h"
#include "mrfi.h"
#include "packetbuf.h"

void mrfi_uip_pkt_convert(void *pPacket, uint16_t pktLen)
{
   packetbuf_copyfrom((const void *)pPacket, pktLen);
}

void mrfi_uip_pkt_proc_done(void)
{
  mrfiPktReceiveFlag = 0; 
}
