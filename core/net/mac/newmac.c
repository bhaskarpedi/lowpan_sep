
/**
 * \name   bmac.c
 * \file
 *        A basic 802.15.4 MAC implementation 
 * \author
 *        Bhaskar Pediredla (pbha564@aucklanduni.ac.nz) 
 */

#include "newmac.h"
#include "net/packetbuf.h"
#include "net/queuebuf.h"

#include "sys/clock.h"
#include "lib/random.h"
#include "net/netstack.h"
#include "lib/list.h"
#include "lib/memb.h"
#include <string.h>

#include <stdio.h>

void init(void)
{
    NETSTACK_RDC.init();
}

void send_packet(mac_callback_t sent_callback, void *ptr)
{
}
 
void input_packet(void)
{
}
  
int on(void)
{
    return NETSTACK_RDC.on();
}

int off(int keep_radio_on)
{
    return NETSTACK_RDC.off(keep_radio_on);
}

unsigned short channel_check_interval(void)
{
}
const struct mac_driver bmac_driver = {
  "newmac",
  init,
  send_packet,
  input_packet,
  on,
  off,
  channel_check_interval,
};
/*---------------------------------------------------------------------------*/
#if 0 /* Added for quick reference */
/**
 * The structure of a MAC protocol driver in Contiki.
 */
struct mac_driver {
  char *name;

  /** Initialize the MAC driver */
  void (* init)(void);

  /** Send a packet from the Rime buffer  */
  void (* send)(mac_callback_t sent_callback, void *ptr);

  /** Callback for getting notified of incoming packet. */
  void (* input)(void);
  
  /** Turn the MAC layer on. */
  int (* on)(void);

  /** Turn the MAC layer off. */
  int (* off)(int keep_radio_on);

  /** Returns the channel check interval, expressed in clock_time_t ticks. */
  unsigned short (* channel_check_interval)(void);
};

/* Generic MAC return values. */
enum {
  /**< The MAC layer transmission was OK. */
  MAC_TX_OK,

  /**< The MAC layer transmission could not be performed due to a
     collision. */
  MAC_TX_COLLISION,

  /**< The MAC layer did not get an acknowledgement for the packet. */
  MAC_TX_NOACK,

  /**< The MAC layer deferred the transmission for a later time. */
  MAC_TX_DEFERRED,

  /**< The MAC layer transmission could not be performed because of an
     error. The upper layer may try again later. */
  MAC_TX_ERR,

  /**< The MAC layer transmission could not be performed because of a
     fatal error. The upper layer does not need to try again, as the
     error will be fatal then as well. */
  MAC_TX_ERR_FATAL,
};
#endif

