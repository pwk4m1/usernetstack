/* Helpers for our socket operations
 *
 */
#ifndef __NETLIB_SOCKET_H__
#define __NETLIB_SOCKET_H__

#include <sys/types.h>
#include <stdint.h>

/*
 * @member int raw_sockfd        -- Socket file descriptor to use
 * @member int family            -- Socket family (AF_INET, AF_INET6, ...)
 * @member int protocol          -- Protocol to use (TCP/UDP/ICMP/..)
 * @member void *link_options    -- Link layer options
 * @member void *ip_options      -- ipv4 or ipv6 options structure
 * @member void *ptcl_options    -- Protocol specific options structure
 * @member char *iface           -- Name of interface to use
 *
 */
typedef struct {
    int raw_sockfd;
    int family;
    int protocol;
    void *link_options;
    void *ip_options;
    void *proto_options;
    char *iface;
} net_socket;

/* Open a raw network socket for user
 *
 * @param const char *iface -- Name of interface to use
 * @return int socket on success or -1 on error.
 */
int raw_socket(const char *iface);

/* Open new network socket for user.
 *
 * @param int family          -- AF_INET/AF_INET6/...
 * @param int protocol        -- TCP/UDP/...
 * @param enum LINK_TYPE type -- ETH/SLIP/PPP/..
 * @param const uint8_t *smac -- Source Mac address
 * @param const uint8_t *dmac -- Source Mac address
 * @param char *iface         -- Name of network interface we're using
 * @return pointer to populated net_socket structure on success or 0 on error.
 * set errno on error.
 */
net_socket *new_socket(int family, int protocol, int type,
        uint8_t *smac, uint8_t *dmac, char *iface);

/* Send up to size_t bytes of data
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to data to transmit
 * @param size_t len       -- Amount of bytes to send
 * @return amount of bytes written on success or -1 on error.
 *         set errno on error.
 */
size_t transmit(net_socket *sock, const void *data, size_t len);

#endif // __NETLIB_SOCKET_H__
