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

/* Implement ip/{tcp,udp} checksum calculation */

#include <sys/types.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include <csum.h>

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
        size_t len)
{
    ipv4_psd_hdr *ret = calloc(1, sizeof(ipv4_psd_hdr));
    assert(ret && "Unable to allocate memory for pseudo header\n");

    ret->src  = src;
    ret->dst  = dst;
    ret->zero = (uint8_t)0;
    ret->ptcl = ptcl;
    ret->len = (uint16_t)(len & 0x0000FFFF);
    return ret;
}

/* Calculate 16 bit checksum for tcp/udp. This is ones' complement of the
 * ones sum of all 16-bit words in psd_hdr and pkt_hdr. 
 *
 * @param uint16_t *data -- Pointer to data from which we'll calculate our csum
 * @param size_t size -- Size of data in bytes
 */
uint16_t csum(uint16_t *data, size_t size) {
    unsigned long sum = 0;

    while (size > 1) {
        sum += *data++;
        size -= 2;
    }
    if (size > 0) {
        // pad with 0 to match 16 bit boundaries
        uint16_t t = (*(uint8_t*)data) << 8;
        sum += t;
    }

    while (sum >> 16) {
        sum = (sum & 0x0000ffff) + (sum >> 16);
    }
    sum = (uint16_t)~sum;
    return (uint16_t)sum;
}

