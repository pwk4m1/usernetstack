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

/* Implement ip/{tcp, udp} header checksum calculation functionality.
 *
 */
#ifndef __NETLIB_CSUM_H__
#define __NETLIB_CSUM_H__

#include <sys/types.h>
#include <stdint.h>

/* ipv{4,6}_psd_hdr structures are pseudo headers that are needed by
 * TCP and UDP checksum calculations. I'd really love not to have TCP/UDP be
 * aware of things on IP layer, but unfortunately that's not possible :(
 *
 * @member uint32_t  src -- Source IPv4 address
 * @member uint32_t  dst -- Destination IPv4 address
 * @member uint8_t  zero -- Needs to be zero
 * @member uint8_t  ptcl -- Protocol identification (TCP or UDP?)
 * @member uint16_t len  -- Length of {TCP,UDP} header + data 
 */
typedef struct {
    uint32_t src;
    uint32_t dst;
    uint8_t  zero;
    uint8_t  ptcl;
    uint16_t len;
} ipv4_psd_hdr;

/* ipv{4,6}_psd_hdr structures are pseudo headers that are needed by
 * TCP and UDP checksum calculations. I'd really love not to have TCP/UDP be
 * aware of things on IP layer, but unfortunately that's not possible :(
 *
 * @member uint64_t src_low      -- Low 64 bits of source IPv6 address
 * @member uint64_t src_high     -- High 64 bits of source IPv6 address
 * @member uint64_t dst_low      -- Low 64 bits of destination IPv6 address
 * @member uint64_t dst_high     -- High 64 bits of destination IPv6 address
 * @member uint32_t len          -- Length of {TCP,UDP} header + data
 * @member uint16_t zero_low     -- Low 16 bits of 24-bit zero field
 * @member uint8_t  zero_high    -- high 8 bits of 24-bit zero field
 * @member uint8_t  ptcl         -- Protocol identification (TCP or UDP?)
 */
typedef struct {
    uint64_t src_low;
    uint64_t src_high;
    uint64_t dst_low;
    uint64_t dst_high;
    uint32_t len;
    uint16_t zero_low;
    uint8_t  zero_high;
    uint8_t  ptcl;
} ipv6_psd_hdr;

/* Craft a ipv4 pseudo header from source/destination sockaddr_in structures
 *
 * @param uint32_t src            -- Source address to use
 * @param uint32_t dst            -- Destination address to use
 * @param struct sockaddr_in *src -- Pointer source sockaddr_in
 * @param struct sockaddr_in *dst -- Pointer to destination sockaddr_in
 * @param uint8_t ptcl            -- Protocol identifier
 * @param size_t len              -- Size of payload to transmit
 * @return pointer to populated ipv4_psd_hdr
 */
ipv4_psd_hdr *craft_ipv4_psd_hdr(uint32_t src,
        uint32_t dst, uint8_t ptcl,
        size_t len);

/* Calculate 16 bit checksum for ip/tcp/udp. This is ones' complement of the
 * ones sum of all 16-bit words in:
 *     ip header on IP layer
 *     pseudo header + current header + payload on TCP/UDP layer 
 *
 * @param uint16_t *data -- Pointer to data from which we'll calculate our csum
 * @param size_t size -- Size of data
 */
uint16_t csum(uint16_t *data, size_t size);

#endif /* __NETLIB_CSUM_H__ */
