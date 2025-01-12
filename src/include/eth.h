/* Helper functions to build us ethernet header
 *
 */
#ifndef __NETLIB_ETH_H__
#define __NETLIB_ETH_H__

#include <sys/types.h>
#include <stdint.h>

#include <socket.h>

/* This structure defines ethernet header content.
 * We're having two 6-byte MAC addresses, and 2-byte 
 * protocol field.
 *
 * @member uint8_t mac_dst -- Destination MAC address
 * @member uint8_t mac_src -- Source MAC address
 * @member uint16_t ptcl   -- Protocol identifier
 */
typedef struct __attribute__((packed)) {
    uint8_t mac_dst[6];
    uint8_t mac_src[6];
    uint16_t ptcl;
} eth_hdr;

/* Create ethernet header with given source and destination MAC addresses
 * and protocol type
 *
 * @param uint8_t *src      -- Pointer to source mac address
 * @param uint8_t *dst      -- Pointer to destination mac address
 * @param uint16_t proto    -- Protocol to use
 * @return pointer to populated ethernet header
 */
eth_hdr *create_eth_hdr(uint8_t *src, uint8_t *dst, uint16_t proto);

/* Transmit datagram over ethernet.
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to protocol headers and data above this layer
 * @param size_t len       -- Amount of bytes to send
 * @return size_t bytes sent on success or -1 on error.
 *         Set errno on error.
 */
size_t eth_transmit(net_socket *sock, const void *data, size_t len);

#endif
