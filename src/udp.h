/* Helpers for crafting udp header, and reading/writing data over udp. */
#ifndef __NETLIB_UDP_H__
#define __NETLIB_UDP_H__

#include <sys/types.h>
#include <netinet/udp.h>
#include <stdint.h>

#include "data_util.h"

/* Create udp header for user.
 *
 * @param struct sockaddr *src -- Pointer to populated source sockaddr structure
 * @param struct sockaddr *dst -- Pointer to populated dest. sockaddr structure
 * @param uint8_t *data        -- Pointer to user data to write
 * @param uint16_t len         -- Amount of data we'll be sending
 * @return Pointer to populated struct udphdr
 */
struct udphdr *create_udp_hdr(struct sockaddr *src, struct sockaddr *dst,
        uint8_t *data, uint16_t len);

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
        uint16_t sport, uint16_t dport, uint8_t *data, size_t len);

#endif // __NETLIB_UDP_H__
