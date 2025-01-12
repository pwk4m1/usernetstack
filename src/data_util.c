/* Helper functions for dealing with various data and stuff :3 
 *
 */
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#include <assert.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ip.h"
#include "data_util.h"


/* Open new network socket for user.
 *
 * @param int family          -- AF_INET/AF_INET6/...
 * @param int protocol        -- TCP/UDP/...
 * @param const uint8_t *smac -- Source Mac address
 * @param const uint8_t *dmac -- Source Mac address
 * @param char *iface         -- Name of network interface we're using
 * @return pointer to populated net_socket structure on success or 0 on error.
 * set errno on error.
 */
net_socket *new_socket(int family, int protocol, uint8_t *smac, uint8_t *dmac, char *iface) {
    net_socket *ret = calloc(1, sizeof(net_socket));
    if (!ret) {
        return 0;
    }

    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock == -1) {
        // errno is set to us by socket()
        free(ret);
        return 0;
    }
    ret->raw_sockfd = sock;
    ret->family = family;
    ret->protocol = protocol;

    ret->link_saddr = calloc(1, sizeof(struct sockaddr_ll));
    if (!ret->link_saddr) {
        free(ret);
        return 0;
    }

    ret->link_daddr = calloc(1, sizeof(struct sockaddr_ll));
    if (!ret->link_saddr) {
        free(ret->link_saddr);
        free(ret);
        return 0;
    }
    
    unsigned int iface_idx = if_nametoindex(iface);
    ret->link_saddr->sll_family = AF_PACKET;
    ret->link_saddr->sll_protocol = htons(ETH_P_ALL);
    ret->link_saddr->sll_ifindex = iface_idx;
    ret->link_saddr->sll_hatype = 1;
    ret->link_saddr->sll_pkttype = PACKET_OTHERHOST;
    ret->link_saddr->sll_halen = ETH_ALEN;
    // 56:94:9d:02:2e:43
    // memcpy(ret->link_saddr->sll_addr, "\x56\x94\x9d\x02\x2e\x43", ETH_ALEN);
    //
    memcpy(ret->link_saddr->sll_addr, smac, ETH_ALEN);

    ret->link_daddr->sll_family = AF_PACKET;
    ret->link_daddr->sll_protocol = htons(ETH_P_ALL);
    ret->link_daddr->sll_ifindex = iface_idx;
    ret->link_daddr->sll_hatype = 1;
    ret->link_daddr->sll_pkttype = PACKET_OTHERHOST;
    ret->link_daddr->sll_halen = ETH_ALEN;
    // fa:22:23:87:a9:9d 
    // memcpy(ret->link_daddr->sll_addr, "\xfa\x22\x23\x87\xa9\x9d", ETH_ALEN);
    //
    memcpy(ret->link_daddr->sll_addr, dmac, ETH_ALEN);

    int stat = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, iface, strlen(iface));
    if (stat == -1) {
        free(ret->link_saddr);
        free(ret);
        return 0;
    }

    ret->ip_options = calloc(1, sizeof(struct ipv4_socket_options));
    if (!ret->ip_options) {
        free(ret->link_saddr);
        free(ret);
        return 0;
    }

    struct ipv4_socket_options *iopts = ret->ip_options;
    iopts->ttl = 64;
    iopts->high_throughput = 1;
    
    return ret;
}

/* Internal helper for building sockaddr_in structure from given user input.
 *
 * @param char *addr        -- Pointer to ipv4 address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 * @return Pointer to a populated sockaddr_in structure
 */
struct sockaddr_in *build_sockaddr_in(char *addr, uint16_t port) {
    struct sockaddr_in *ret = calloc(1, sizeof(struct sockaddr_in));
    assert(ret);
    memset(ret, 0, sizeof(struct sockaddr_in));

    ret->sin_family = AF_INET;
    ret->sin_addr.s_addr = inet_addr((const char *)addr);
    ret->sin_port = htons(port);

    return ret;
}

/* Internal helper for building sockaddr_in6 structure from given user input.
 *
 * @param uint8_t *addr     -- Pointer to ipv4 address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 *
 */

/* Helper to populate sockaddr_in and sockaddr_in6 structures from
 * user provided data.
 *
 * @param int family        -- AF_INET or AF_INET6
 * @param char *addr        -- Pointer to ip address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 * @return pointer to populated struct sockaddr on success, or 0 if family
 *         isn't supported
 */
struct sockaddr *populate_sockaddr(int family, char *addr, uint16_t port) {
    struct sockaddr *ret = NULL;
    switch (family) {
        case (AF_INET):
            ret = (struct sockaddr *)build_sockaddr_in(addr, port);
            break;
        case (AF_INET6):
            ret = 0;
            errno = ENOSYS;
            break;
    }
    return ret;
}


/* Helper to create continuous packet from various protocol header structures
 * and user-provided payload.
 *
 * @param void **pkt    -- Pointer to use for crafting the packet
 * @param void *iphdr   -- Pointer to IP or pseudo IP header
 * @param size_t iplen  -- Size of provided IP or pseudo header
 * @param void *ptclhdr -- UDP or TCP header
 * @param size_t ptclen -- Size of provided UDP or TCP header
 * @param void *payload -- Data to transmit
 * @param size_t len    -- Size of payload to transmit
 * @return size_t size of built packet
 *
 * NOTES: 
 *  *pkt is allocated by this function for the caller.
 *  All headers and data can be set to NULL with 0 as corresponding length
 *  to have them not be included
 */
size_t build_packet(void **pkt, void *iphdr, size_t iplen,
        void *ptclhdr, size_t ptclen,
        void *payload, size_t len)
{
    size_t total = iplen + ptclen + len;
    unsigned char *ret = malloc(total);
    assert(ret && "Unable to allocate memory for full packet\n");

    memcpy(ret, iphdr, iplen);
    memcpy(ret+iplen, ptclhdr, ptclen);
    memcpy(ret+iplen+ptclen, payload, len);

    *pkt = (void *)ret;
    return total;
}

