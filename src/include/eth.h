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
