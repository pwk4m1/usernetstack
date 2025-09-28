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
#ifndef __UNET_UTIL_H__
#define __UNET_UTIL_H__

#include <stdint.h>

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

#endif // __UNET_UTIL_H__
