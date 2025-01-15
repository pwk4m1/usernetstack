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

/* Helpers for crafting udp header, and reading/writing data over udp. */

#include <sys/types.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <data_util.h>
#include <ip.h>
#include <udp.h>
#include <socket.h>

/* Create udp header for user.
 *
 * @param uint16_t sport       -- src port
 * @param uint16_t dport       -- dst port
 * @param uint8_t *data        -- Pointer to user data to write
 * @param uint16_t len         -- Amount of data we'll be sending
 * @return Pointer to populated udp_hdr
 */
udp_hdr *create_udp_hdr(uint16_t sport, uint16_t dport,
        uint8_t *data, uint16_t len)
{
    udp_hdr *ret = (udp_hdr *)calloc(1, sizeof(udp_hdr));
    assert(ret && "Unable to allocate memory for UDP header\n");

    /* The format for both sockaddr_in and sockaddr_in6 is identical
     * for first members (family, sin_port). That's why we can use just
     * sockaddr_in->sin_port here without h aving to worry about 
     * compability issues between ipv4 and ipv6.
     */
    ret->dst = htons(dport);
    ret->src = htons(sport);
    ret->len = htons(len + sizeof(udp_hdr));

    // TODO: UDP Checksums
    ret->csum = htons(0x0000);

    return ret;
}

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
        uint16_t sport, uint16_t dport, uint8_t *data, size_t len)
{
    udp_hdr *uhdr  = create_udp_hdr(sport, dport, data, len);
    if (!uhdr) {
        return 0;
    }
    void *packet = realloc(uhdr, uhdr->len);
    if (!packet) {
        free(uhdr);
        return 0;
    }
    memcpy(POINTER_ADD(void *, uhdr, sizeof(udp_hdr)), data, len);

    size_t sent = ipv4_transmit_datagram(sock, src_addr, dst_addr, packet, (sizeof(udp_hdr) + len));

    free(packet);
    return sent;
}

