/* Helper functions to build us ethernet header
 *
 */
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>

#include <data_util.h>
#include <eth.h>

/* Create ethernet header with given source and destination MAC addresses
 * and protocol type
 *
 * @param net_socket *socket -- Socket we're working with
 * @param uint16_t proto     -- Protocol to use
 * @return pointer to populated ethernet header
 */
eth_hdr *create_eth_hdr(net_socket *socket, uint16_t proto) {
    eth_hdr *ret = (eth_hdr *)calloc(1, sizeof(eth_hdr));
    if (!ret) {
        return ret;
    }
    memcpy(ret->mac_src, socket->mac_src, 6);
    memcpy(ret->mac_dst, socket->mac_dst, 6);
    ret->ptcl = htons(proto);
    return ret;
}

/* Transmit datagram over ethernet.
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to protocol headers and data above this layer
 * @param uint16_t len     -- Amount of bytes to send
 * @return uint16_t bytes sent on success or -1 on error.
 *         Set errno on error.
 */
uint16_t eth_transmit_frame(net_socket *sock, const void *data, uint16_t len) {
    uint16_t sent = -1;
    eth_hdr *eth = create_eth_hdr(sock, 0x0800);
    if (!eth) {
        return sent;
    }

    void *packet = calloc(1, (sizeof(eth_hdr) + len));
    if (!packet) {
        return sent;
    }
    memcpy(packet, eth, sizeof(eth_hdr));
    memcpy(POINTER_ADD(void *, packet, sizeof(eth_hdr)), data, len);

    sent = transmit(sock, (const void *)packet, (sizeof(eth_hdr) + len));

    free(packet);
    free(eth);
    return sent;
}

