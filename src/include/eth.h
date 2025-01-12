/* Helper functions to build us ethernet header
 *
 */
#ifndef __NETLIB_ETH_H__
#define __NETLIB_ETH_H__

// #include <net/ethernet.h>
//
#include <sys/types.h>
#include <stdint.h>

#include "socket.h"

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
 * @param net_socket *socket -- Socket we're working with
 * @param uint16_t proto    -- Protocol to use
 * @return pointer to populated ethernet header
 */
eth_hdr *create_eth_hdr(net_socket *socket, uint16_t proto);

/* Transmit datagram over ethernet.
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to protocol headers and data above this layer
 * @param uint16_t len     -- Amount of bytes to send
 * @return uint16_t bytes sent on success or -1 on error.
 *         Set errno on error.
 */
uint16_t eth_transmit_frame(net_socket *sock, const void *data, uint16_t len);

#endif
