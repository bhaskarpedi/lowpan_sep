#if 0
#include "lowpan.h"

/* 16 bit address obtained after Association */
#define IEEE802154_SHORTADDR_LEN 2

/* 64 bit EUID */
//#define IEEE802154_LONGADDR_LEN 8

typedef struct {
	uint8_t u8[IEEE802154_SHORTADDR_LEN];
} rimeaddr_t;

/*
typedef struct ieeeLongAddr{
	uint8_t u8[IEEE802154_LONGADDR_LEN];
}ieeeLongAddr_t;
*/

/**
 * \brief      Copy a Rime address
 * \param dest The destination
 * \param from The source
 *
 *             This function copies a Rime address from one location
 *             to another.
 *
 */
void rimeaddr_copy(rimeaddr_t *dest, const rimeaddr_t *from);

/**
 * \brief      Compare two Rime addresses
 * \param addr1 The first address
 * \param addr2 The second address
 * \return     Non-zero if the addresses are the same, zero if they are different
 *
 *             This function compares two Rime addresses and returns
 *             the result of the comparison. The function acts like
 *             the '==' operator and returns non-zero if the addresses
 *             are the same, and zero if the addresses are different.
 *
 */
int rimeaddr_cmp(const rimeaddr_t *addr1, const rimeaddr_t *addr2);


/**
 * \brief      Set the address of the current node
 * \param addr The address
 *
 *             This function sets the Rime address of the node.
 *
 */
void rimeaddr_set_node_addr(rimeaddr_t *addr);

/**
 * \brief      The Rime address of the node
 *
 *             This variable contains the Rime address of the
 *             node. This variable should not be changed directly;
 *             rather, the rimeaddr_set_node_addr() function should be
 *             used.
 *
 */
extern rimeaddr_t rimeaddr_node_addr;

/**
 * \brief      The null Rime address
 *
 *             This variable contains the null Rime address. The null
 *             address is used in route tables to indicate that the
 *             table entry is unused. Nodes with no configured address
 *             has the null address. Nodes with their node address set
 *             to the null address will have problems communicating
 *             with other nodes.
 *
 */
extern const rimeaddr_t rimeaddr_null;

#endif
