/* Helper functions to build us ethernet header
 *
 */
#include <sys/types.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

#include <linux/if_packet.h>

#include <stdlib.h>
#include <string.h>

#include "data_util.h"
#include "eth.h"

/* Create ethernet header with given source and destination MAC addresses
 * and protocol type
 *
 * @param net_socket *socket -- Socket we're working with
 * @param uint16_t proto     -- Protocol to use
 * @return pointer to populated ethernet header
 */
struct ethhdr *create_eth_hdr(net_socket *socket, uint16_t proto) {
    struct ethhdr *ret = calloc(1, sizeof(struct ethhdr));
    if (!ret) {
        return ret;
    }
    memcpy(ret->h_source, socket->link_saddr->sll_addr, 6);
    memcpy(ret->h_dest, socket->link_daddr->sll_addr, 6);

    ret->h_proto = htons(proto);
    return ret;
}

