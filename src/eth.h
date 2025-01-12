/* Helper functions to build us ethernet header
 *
 */
#ifndef __NETLIB_ETH_H__
#define __NETLIB_ETH_H__

#include <net/ethernet.h>

#include "data_util.h"

/* Create ethernet header with given source and destination MAC addresses
 * and protocol type
 *
 * @param net_socket *socket -- Socket we're working with
 * @param uint16_t proto    -- Protocol to use
 * @return pointer to populated ethernet header
 */
struct ethhdr *create_eth_hdr(net_socket *socket, uint16_t proto);

#endif
