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

