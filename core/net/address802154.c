#if 0
#include "lowpan.h"
#include "address802154.h"

rimeaddr_t rimeaddr_node_addr;
const rimeaddr_t rimeaddr_null = { { 0, 0 } };

/*---------------------------------------------------------------------------*/
void rimeaddr_copy(rimeaddr_t *dest, const rimeaddr_t *src)
{
  uint8_t i;
  for(i = 0; i < IEEE802154_SHORTADDR_LEN; i++) {
    dest->u8[i] = src->u8[i];
  }
}
/*---------------------------------------------------------------------------*/
int rimeaddr_cmp(const rimeaddr_t *addr1, const rimeaddr_t *addr2)
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
void rimeaddr_set_node_addr(rimeaddr_t *t)
{
  rimeaddr_copy(&rimeaddr_node_addr, t);
}
/*---------------------------------------------------------------------------*/
/** @} */
#endif
