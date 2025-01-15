/*
 BSD 3-Clause License
 
 Copyright (c) 2025, k4m1 <me@k4m1.net>
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
