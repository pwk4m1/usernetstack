/* Helpers for crafting udp header, and reading/writing data over udp. */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "csum.h"
#include "ip.h"
#include "udp.h"

/* Create udp header for user.
 *
 * @param struct sockaddr *src -- Pointer to populated source sockaddr structure
 * @param struct sockaddr *dst -- Pointer to populated dest. sockaddr structure
 * @param uint8_t *data        -- Pointer to user data to write
 * @param uint16_t len         -- Amount of data we'll be sending
 * @return Pointer to populated struct udphdr
 */
struct udphdr *create_udp_hdr(struct sockaddr *src, struct sockaddr *dst,
        uint8_t *data, uint16_t len)
{
    uint16_t sum;
    struct udphdr *ret = (struct udphdr *)calloc(1, sizeof(struct udphdr));
    assert(ret);

    /* The format for both sockaddr_in and sockaddr_in6 is identical
     * for first members (family, sin_port). That's why we can use just
     * sockaddr_in->sin_port here without h aving to worry about 
     * compability issues between ipv4 and ipv6.
     */
    struct sockaddr_in *s_src = (struct sockaddr_in *)src;
    struct sockaddr_in *s_dst = (struct sockaddr_in *)dst;
    ret->uh_dport = s_dst->sin_port;
    ret->uh_sport = s_src->sin_port;
    ret->uh_ulen = htons(len + sizeof(struct udphdr));

    if (s_src->sin_family == AF_INET) {
        ipv4_psd_hdr *hdr = craft_ipv4_psd_hdr(s_src, 
                s_dst, 
                len + sizeof(struct udphdr));
        void *pkt = 0;
        size_t plen = build_packet(&pkt, hdr, sizeof(ipv4_psd_hdr),
                ret, sizeof(struct udphdr), data, len);
        //sum = csum(pkt, plen);
        sum = 0; // TODO: Fix later :3
        free(pkt);
    } else {
        // TODO: Support ipv6 lol
        abort();
    }
    ret->uh_sum = sum;

    return ret;
}

/* Send a message over UDP to a remote host
 *
 * @param int sockfd           -- Raw socket descriptor to use for sending data
 * @param int family           -- is this ipv4 or ipv6
 * @param char *saddr          -- Pointer to string create_udp_hdr of src IP address
 * @param char *daddr          -- Pointer to string representation of dst IP address
 * @param uint16_t sport       -- UDP Port to send our data from
 * @param uint16_t dport       -- UDP Port to send our data to
 * @param uint8_t *data        -- Pointer to data to transmit
 * @param size_t len           -- Amount of bytes to send
 * @return size_t bytes sent
 */
size_t udp_send(int sockfd, int family, char *saddr, char *daddr, 
        uint16_t sport, uint16_t dport, uint8_t *data, size_t len)
{
    void *packet = 0;
    struct sockaddr *src = populate_sockaddr(family, saddr, sport);
    struct sockaddr *dst = populate_sockaddr(family, daddr, dport);
    struct iphdr *iphr = create_std_ipv4_hdr((struct sockaddr_in *)src, 
            (struct sockaddr_in *)dst, 17, len + sizeof(struct udphdr));
    struct udphdr *uhdr  = create_udp_hdr(src, dst, data, len);
    size_t plen = build_packet(&packet, iphr, sizeof(struct iphdr), 
            uhdr, sizeof(struct udphdr),
            data, len);
    iphr->check = csum(packet, plen);
    size_t stat = sendto(sockfd, packet, plen, 0, dst, sizeof(struct sockaddr));
    free(packet);
    return stat;
}


