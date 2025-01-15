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

/* Helper functions for dealing with various data and stuff :3 
 *
 */
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>

#include <data_util.h>

/* Create uint32_t ipv4 address from
 * string representation
 *
 * @param char *ip -- Pointer to IP address string
 * @return uint32_t address
 */
uint32_t inet_addr(const char *ip) {
    uint32_t ret = 0;
    char *tmp = strdup(ip);
    char *tok = strtok(tmp, ".");
    if (!tok) {
        return 0;
    }

    for (int i = 0; i < 4; i++) {
        uint8_t val = atoi(tok);
        ret |= (val << (i * 8));
        tok = strtok(NULL, ".");
    }
    free(tmp);
    return ret;
}

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
 * NOTES: 
 *  *pkt is allocated by this function for the caller.
 *  All headers and data can be set to NULL with 0 as corresponding length
 *  to have them not be included
 */
size_t build_packet(void **pkt, void *iphdr, size_t iplen,
        void *ptclhdr, size_t ptclen,
        void *payload, size_t len)
{
    size_t total = iplen + ptclen + len;
    unsigned char *ret = malloc(total);
    assert(ret && "Unable to allocate memory for full packet\n");

    memcpy(ret, iphdr, iplen);
    memcpy(ret+iplen, ptclhdr, ptclen);
    memcpy(ret+iplen+ptclen, payload, len);

    *pkt = (void *)ret;
    return total;
}

