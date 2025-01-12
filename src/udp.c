/* Helpers for crafting udp header, and reading/writing data over udp. */

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <data_util.h>
#include <ip.h>
#include <udp.h>
#include <socket.h>

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
    struct udphdr *ret = (struct udphdr *)calloc(1, sizeof(struct udphdr));
    assert(ret && "Unable to allocate memory for UDP header\n");

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

    // TODO: UDP Checksums
    ret->uh_sum = htons(0x0000);

    return ret;
}

/* Send a message over UDP to a remote host
 *
 * @param net_socket *sock     -- Pointer to populated net_socket structure
 * @param char *saddr          -- Pointer to string create_udp_hdr of src IP address
 * @param char *daddr          -- Pointer to string representation of dst IP address
 * @param uint16_t sport       -- UDP Port to send our data from
 * @param uint16_t dport       -- UDP Port to send our data to
 * @param uint8_t *data        -- Pointer to data to transmit
 * @param size_t len           -- Amount of bytes to send
 * @return size_t bytes sent
 */
size_t udp_send(net_socket *sock, char *saddr, char *daddr, 
        uint16_t sport, uint16_t dport, uint8_t *data, size_t len)
{
    struct sockaddr *src = populate_sockaddr(sock->family, saddr, sport);
    struct sockaddr *dst = populate_sockaddr(sock->family, daddr, dport);
    struct udphdr *uhdr  = create_udp_hdr(src, dst, data, len);
    if (!uhdr) {
        return 0;
    }
    void *packet = realloc(uhdr, uhdr->uh_ulen);
    if (!packet) {
        free(uhdr);
        return 0;
    }
    memcpy(POINTER_ADD(void *, uhdr, sizeof(struct udphdr)), data, len);

    size_t sent = ipv4_transmit_datagram(sock, (struct sockaddr_in *)src,
            (struct sockaddr_in *)dst, packet, (sizeof(struct udphdr) + len));

    free(packet);
    free(dst);
    free(src);
    return sent;
}

