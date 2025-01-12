/* Helper functions to build us ethernet header
 *
 */
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>

#include <data_util.h>
#include <eth.h>
#include <link.h>


/* Create ethernet header with given source and destination MAC addresses
 * and protocol type
 *
 * @param uint8_t *src      -- Pointer to source mac address
 * @param uint8_t *dst      -- Pointer to destination mac address
 * @param uint16_t proto    -- Protocol to use
 * @return pointer to populated ethernet header
 */
eth_hdr *create_eth_hdr(uint8_t *src, uint8_t *dst, uint16_t proto) {
    eth_hdr *ret = (eth_hdr *)calloc(1, sizeof(eth_hdr));
    if (!ret) {
        return ret;
    }
    memcpy(ret->mac_src, src, 6);
    memcpy(ret->mac_dst, dst, 6);
    ret->ptcl = htons(proto);
    return ret;
}

/* Transmit datagram over ethernet.
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to protocol headers and data above this layer
 * @param size_t len       -- Amount of bytes to send
 * @return size_t bytes sent on success or -1 on error.
 *         Set errno on error.
 */
size_t eth_transmit(net_socket *sock, const void *data, size_t len) {
    uint16_t sent = -1;
    link_options *link = (link_options *)sock->link_options;

    void *packet = calloc(1, (sizeof(eth_hdr) + len));
    if (!packet) {
        return sent;
    }

    memcpy(packet, link->proto.eth_header, sizeof(eth_hdr));
    memcpy(POINTER_ADD(void *, packet, sizeof(eth_hdr)), data, len);

    sent = transmit(sock, (const void *)packet, (sizeof(eth_hdr) + len));

    free(packet);
    return sent;
}

