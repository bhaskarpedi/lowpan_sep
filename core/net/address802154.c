#include "lowpan.h"
#include "address802154.h"

ieeeShortAddr_t ieeeShortAddr_node_addr;
const ieeeShortAddr_t ieeeShortAddr_null = { { 0, 0 } };

/*---------------------------------------------------------------------------*/
void ieeeShortAddr_copy(ieeeShortAddr_t *dest, const ieeeShortAddr_t *src)
{
  uint8_t i;
  for(i = 0; i < IEEE802154_SHORTADDR_LEN; i++) {
    dest->u8[i] = src->u8[i];
  }
}
/*---------------------------------------------------------------------------*/
int ieeeShortAddr_cmp(const ieeeShortAddr_t *addr1, const ieeeShortAddr_t *addr2)
{
  uint8_t i;
  for(i = 0; i < IEEE802154_SHORTADDR_LEN; i++) {
    if(addr1->u8[i] != addr2->u8[i]) {
      return 0;
    }
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
void ieeeShortAddr_set_node_addr(ieeeShortAddr_t *t)
{
  ieeeShortAddr_copy(&ieeeShortAddr_node_addr, t);
}
/*---------------------------------------------------------------------------*/
/** @} */
