/* Helpers for crafting udp header, and reading/writing data over udp. */
#ifndef __NETLIB_UDP_H__
#define __NETLIB_UDP_H__

#include <sys/types.h>
#include <stdint.h>

#include "socket.h"

/* UDP header structure
 *
 *
 */
typedef struct {
    uint16_t src;
    uint16_t dst;
    uint16_t len;
    uint16_t csum;
} udp_hdr;

/* Create udp header for user.
 *
 * @param uint16_t sport       -- src port
 * @param uint16_t dport       -- dst port
 * @param uint8_t *data        -- Pointer to user data to write
 * @param uint16_t len         -- Amount of data we'll be sending
 * @return Pointer to populated struct udphdr
 */
udp_hdr *create_udp_hdr(uint16_t sport, uint16_t dport,
        uint8_t *data, uint16_t len);

/* Send a message over UDP to a remote host
 *
 * @param net_socket *sock     -- Pointer to populated net_socket structure
 * @param uint32_t src_addr    -- Source IP address
 * @param uint32_t dst_addr    -- Destination IP address
 * @param uint16_t sport       -- UDP Port to send our data from
 * @param uint16_t dport       -- UDP Port to send our data to
 * @param uint8_t *data        -- Pointer to data to transmit
 * @param size_t len           -- Amount of bytes to send
 * @return size_t bytes sent
 */
size_t udp_send(net_socket *sock, uint32_t src_addr, uint32_t dst_addr,
        uint16_t sport, uint16_t dport, uint8_t *data, size_t len);

#endif // __NETLIB_UDP_H__
