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

/* Helper structures, definitions, and function declarations that are related
 * to abstracting away varying data types, so that each protocol layer as is
 * independent of eachother as possible, and that read/write, etc. are as 
 * uniform between different protocols.
 *
 */
#ifndef __NETLIB_DATA_UTIL_H__
#define __NETLIB_DATA_UTIL_H__

#include <sys/types.h>

#include <assert.h>
#include <stdint.h>

/* Simple byteswap
 *
 */
inline uint16_t bswap_16(uint16_t in) {
    uint8_t hi = (in & 0xFF00) >> 8;
    uint8_t lo = (in & 0x00FF);
    uint16_t ret = (lo << 8) | hi;
    return ret;
}

inline uint32_t bswap_32(uint32_t in) {
    uint16_t lo = (in & 0x0000FFFF);
    uint16_t hi = (in & 0xFFFF0000) >> 16;

    uint32_t ret = (bswap_16(lo) << 16) | (bswap_16(hi));
    return ret;
}

/* Create uint32_t ipv4 address from
 * string representation
 *
 * @param const char *ip -- Pointer to IP address string
 * @return uint32_t address
 */
uint32_t inet_addr(const char *ip);

/* Swap bytes to network host order
 *
 * @param uint16_t in -- Data to convert
 * @return uint16_t data in network host order
 */
inline uint16_t htons(uint16_t in) {
    return bswap_16(in);
}

/* Swap bytes to network host order
 *
 * @param uint16_t in -- Data to convert
 * @return uint16_t data in host order
 */
inline uint16_t ntohs(uint16_t in) {
    return bswap_16(in);
}

/* Swap bytes to network host order
 *
 * @param uint32_t in -- Data to convert
 * @return uint32_t data in network host order
 */
inline uint32_t htonl(uint16_t in) {
    return bswap_32(in);
}

/* Add 'addition' amount of bytes to orig_ptr, since ptr+adddition is
 * gnu_extension for most data types
 *
 * @param dest_type -- type of pointer we want as a result
 * @param orig_ptr  -- pointer to do arithmetics on
 * @param addition  -- amount of _bytes_ we want to add to orig_ptr address
 */
#define POINTER_ADD(dest_type, orig_ptr, addition) ((dest_type)(((uint64_t)orig_ptr) + addition))

/* Helper to populate sockaddr_in and sockaddr_in6 structures from
 * user provided data.
 *
 * @param int family        -- AF_INET or AF_INET6
 * @param char *addr        -- Pointer to ip address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 * @return pointer to populated struct sockaddr on success or 
 *                              0 if family isn't supported yet.
 *                              Set errno on error.
 */
struct sockaddr *populate_sockaddr(int family, char *addr, uint16_t port);

/* Helper to create continuous packet from various protocol header structures
 * and user-provided payload.
 *
 * @param void **pkt    -- Pointer to use for crafting the packet
 * @param void *iphdr   -- Pointer to IP or pseudo IP header
 * @param size_t iplen  -- Size of provided IP or pseudo header
 * @param void *ptclhdr -- UDP or TCP header
 * @param size_t ptclen -- Size of provided UDP or TCP header
 * @param void *payload -- Data to transmit
 * @param size_t len    -- Size of payload to transmit
 * @return size_t size of built packet
 *
 * NOTE: *pkt is allocated by this function for the caller
 */
size_t build_packet(void **pkt, void *iphdr, size_t iplen,
        void *ptclhdr, size_t ptclen,
        void *payload, size_t len);

#endif // __NETLIB_DATA_UTIL_H__
