/* Helpers for our socket operations
 *
 */

#include <sys/types.h>

#include <stdlib.h>

#include <ip.h>
#include <link.h>
#include <socket.h>

/* Open new network socket for user.
 *
 * @param int family          -- AF_INET/AF_INET6/...
 * @param int protocol        -- TCP/UDP/...
 * @param enum LINK_TYPE type -- ETH/SLIP/PPP/..
 * @param const uint8_t *smac -- Source Mac address
 * @param const uint8_t *dmac -- Source Mac address
 * @param char *iface         -- Name of network interface we're using
 * @return pointer to populated net_socket structure on success or 0 on error.
 *         set errno on error.
 */
net_socket *new_socket(int family, int protocol, int type,
        uint8_t *smac, uint8_t *dmac, char *iface) {
    net_socket *ret = (net_socket *)calloc(1, sizeof(net_socket));
    if (!ret) {
        return 0;
    }
    ret->raw_sockfd = raw_socket(iface);
    ret->iface = iface;

    ret->family = family;
    ret->protocol = protocol;

    ret->ip_options = calloc(1, sizeof(ipv4_socket_options));
    if (!ret->ip_options) {
        free(ret);
        return 0;
    }

    ipv4_socket_options *iopts = (ipv4_socket_options *)ret->ip_options;
    iopts->ttl = 64;
    iopts->high_throughput = 1;

    ret->link_options = calloc(1, sizeof(link_options));
    if (!ret->link_options) {
        free(ret->ip_options);
        free(ret);
        return 0;
    }
    link_options *link = (link_options *)ret->link_options;
    link->type = type;

    switch (type) {
    case (ETH):
        link->proto.eth_header = create_eth_hdr(smac, dmac, 0x0800);
        break;
    case (SLIP):
        link->proto.slip_port = 0x02f8;
        break;
    }

    return ret;
}


