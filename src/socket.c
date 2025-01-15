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

#include <sys/types.h>

#include <stdlib.h>

#include <ip.h>
#include <link.h>
#include <socket.h>

/* Open new network socket for user.
 *
 * @param int family          -- AF_INET/AF_INET6/...
 * @param int protocol        -- TCP/UDP/...
 * @param enum LINK_TYPE type -- ETH/SLIP/PPP/..
 * @param const uint8_t *smac -- Source Mac address
 * @param const uint8_t *dmac -- Source Mac address
 * @param char *iface         -- Name of network interface we're using
 * @return pointer to populated net_socket structure on success or 0 on error.
 *         set errno on error.
 */
net_socket *new_socket(int family, int protocol, int type,
        uint8_t *smac, uint8_t *dmac, char *iface) {
    net_socket *ret = (net_socket *)calloc(1, sizeof(net_socket));
    if (!ret) {
        return 0;
    }
    ret->raw_sockfd = raw_socket(iface);
    ret->iface = iface;

    ret->family = family;
    ret->protocol = protocol;

    ret->ip_options = calloc(1, sizeof(ipv4_socket_options));
    if (!ret->ip_options) {
        free(ret);
        return 0;
    }

    ipv4_socket_options *iopts = (ipv4_socket_options *)ret->ip_options;
    iopts->ttl = 64;
    iopts->high_throughput = 1;

    ret->link_options = calloc(1, sizeof(link_options));
    if (!ret->link_options) {
        free(ret->ip_options);
        free(ret);
        return 0;
    }
    link_options *link = (link_options *)ret->link_options;
    link->type = type;

    switch (type) {
    case (ETH):
        link->proto.eth_header = create_eth_hdr(smac, dmac, 0x0800);
        break;
    case (SLIP):
        link->proto.slip_port = 0x02f8;
        break;
    }

    return ret;
}


