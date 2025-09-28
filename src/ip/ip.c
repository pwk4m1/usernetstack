/*
 BSD 3-Clause License
 
 Copyright (c) 2025, k4m1  <me@k4m1.net>
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
#include <sys/types.h>
#include <stdint.h>

#include "ip.h"
#include "../route/route.h"
#include "../iface/iface.h"
#include "../buffer/buffer.h"

/**
 * Helper to populate a default/standard IPv4 header
 *
 *
 */
static void populate_ipv4hdr(ipv4_header *hdr, uint32_t src, uint32_t dst, uint8_t ptcl, uint16_t len) {
    hdr->src = src;
    hdr->dst = dst;
}

/**
 * Send a packet over a IPv4 to a given destination address.
 *
 * @param host is the destination address
 * @parma ptcl is the protocol identifier to use
 * @param data is a pointer to buffer of data to transmit
 * @param src is optional source address. Set to NULL for default
 * @param options is a pointer to a populated ipv4_option_structure or NULL for none 
 * @return uint16_t amount of bytes sent or 0 on error. Sets errno on error.
 */
uint16_t ipv4_transmit(uint32_t host, uint8_t ptcl, buffer *data, uint32_t src, ipv4_option_structure *options) {
    ipv4_route_entry *route = ipv4_get_route_to_host(host);
    if (!route) {
        return 0;
    }
    if (options) {
        errno = ENOSYS;
        return 0;
    }
    buffer *packet = new_buffer(sizeof(ipv4_header) + data->len);
    if (!packet) {
        return 0;
    }
    populate_ipv4hdr((ipv4_header *)packet->buf, host, route->address, ptcl, data->len);
    append_buffer(packet, data);
    uint16_t ret = route->iface->link->tx(route->iface->link, packet);
    free(packet);
    return ret;
}


