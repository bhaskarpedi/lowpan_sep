/*
 * main.c
 */
#include "frame802154.h" 
#include "memb.h"
#include "netstack.h"
#include "lowpan.h"
#include "bsp.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"
#include "uip.h"
#include "uip-icmp6.h"
#include "mrfi_uip_if.h"

#define DELAY_IN_MSECS(x) {\
   int d_cnt;\
   for(d_cnt=0;d_cnt=1000;d_cnt++)\
   {\
      BSP_DELAY_USECS(x);\
   }\
}

#define UIP_ICMP_BUF            ((struct uip_icmp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#define PING_DATALEN 16
#define MAX_PING_CNT 10

void uip_ping6(uip_ipaddr_t *dest)
{
   static uint16_t pingByte = 0;
   
   /* set identifier and sequence number to 0 */
   memset((uint8_t *)UIP_ICMP_BUF + UIP_ICMPH_LEN, 0, 4);
   /* put one byte of data */
   memset((uint8_t *)UIP_ICMP_BUF + UIP_ICMPH_LEN + UIP_ICMP6_ECHO_REQUEST_LEN,
         pingByte++, PING_DATALEN);
   uip_icmp6_send(&dest, ICMP6_ECHO_REQUEST, 0,\
                                       UIP_ICMP6_ECHO_REQUEST_LEN + PING_DATALEN);

   /* Rounding off pingByte */
   if(pingByte >= MAX_PING_CNT) pingByte = 0;
}

void main(void) 
{
   int i;
   BSP_Init();
   netstack_init();
   void **pPacket = NULL;
   uint16_t pktLen;
   uip_ipaddr_t dest = {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
      0x12, 0x23, 0x34, 0x00, 0x00, 0x00, 0x00, 0x01}; 
#if ((1 == MODE_PING_TEST) && ((1 == MODE_TX)||(1 == MODE_TX)))
      #error PING and UDP cannot be enabled simultaneously
#endif
#if (1 == MODE_PING_TEST)
   while(1)
   {
      /* Ping the specified destination 10 times every time the button
       * is pressed */
      
      /* Turn on LED1 to indicate that node is ready for button press */
      BSP_TURN_ON_LED1();

      if(BSP_BUTTON1())
      {
         int pingCnt;
         for(pingCnt=0; pingCnt<MAX_PING_CNT; pingCnt++)
         {
            uip_ping6(&dest);
            DELAY_IN_MSECS(1000);
         }
      }

      /* Turn off LED1 to indicate that node is operating on Rx chain*/
      BSP_TURN_OFF_LED1();
      
      if(1 == NETSTACK_RADIO.pending_packet())
      {
         pktLen = NETSTACK_RADIO.read((void*)pPacket, pktLen);
         mrfi_uip_pkt_convert(*pPacket, pktLen); 
         NETSTACK_RDC.input();
         mrfi_uip_pkt_proc_done();
      }

      /*
      if(1 == mrfi_pkt_tx_pend)
      {
         // The response packet, if any, should be prepared in the RX chain
         tcpip_ipv6_output();
      }
      */
   }
#endif

#if (1 == MODE_TX)
   /*TODO: UDP send comes here */
#elif (1 == MODE_RX)
   /* TODO: UDP Receive comes here and packet processing happens from here */
#endif

   while(1)
   {
      BSP_TURN_OFF_LED1();
      if(BSP_BUTTON1()||BSP_BUTTON2())
      {
         BSP_TURN_ON_LED1();
         for(i=0;i<1000;i++)
            BSP_DELAY_USECS(1000);
      }
   }
}
