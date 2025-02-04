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

/* Helpers for creating IPv4 and IPv6 headers */
#include <sys/types.h>
#include <assert.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//#include <threads.h>

#include <bitmap.h>
#include <csum.h>
#include <data_util.h>
#include <link.h>
#include <ip.h>

// This bitmask contains those ip id values that are
// currently in use.
static bitmap_t used_ip_id_values = 0;

// Last block we used for selecting ip id from
static uint16_t last_block = 0;

// maximum amout of blocks present
uint16_t max_blocks = 1024;

// Mutex lock for id bitmask
// mtx_t id_map_lock;

/* Initialise ip header system */
void ip_initialise(void) {
   // mtx_init(&id_map_lock, 0);
    srand(((uint64_t)&ip_initialise) % 0xffff);
    last_block = rand() % 0x1000; 
    used_ip_id_values = calloc(1, (65536 / 8));
    memset(used_ip_id_values, 0, (65536 / 8));
    assert(used_ip_id_values && "Could not allocate memory for ip id management");
}

/* Finalise ip header system */
void ip_finalise(void) {
    //mtx_lock(&id_map_lock);
    //mtx_destroy(&id_map_lock);
    free(used_ip_id_values);
}

/* Select unique IPv4 ID field value as per RFC 6864[1]. 
 * 
 * The current idea is, that each IPv4 "stream"[2] gets one ID value
 * when create_..._ipv4_hdr(...) is called. The ID value is 'freed' once
 * destroy_ipv4_hdr(...) is called after all datagrams have been transmitted.
 * 
 * [1]: https://www.rfc-editor.org/rfc/rfc6864
 * [2]: With the stream here, we don't refer to TCP stream, but rather 
 *      IPv4 datagrams that are fragmented for whatever reason.
 *
 * @param struct iphdr *iph -- Pointer to populated IPv4 header, that
 *                             has no ID or checksum fields filled yet.
 * @return bool true on success or false on error.
 */
static bool allocate_ipv4_id(ipv4_hdr *iph) {
    //int mutex_result;

    /* Try and lock mutex, if result isn't success or busy,
     * we've errored out and should return false.
     */
    /*
    do {
        mutex_result = mtx_trylock(&id_map_lock);
    } while (mutex_result == thrd_busy);
    if (mutex_result != thrd_success) {
        return false;
    }
    */

    /* Select a random starting point */
    srand(last_block);
    uint16_t current_block = (last_block + rand()) % max_blocks;
    uint16_t entry = (current_block * 8);
    uint16_t current_id = 0;
    last_block = current_block;

    do {
        if (bitmap_get(used_ip_id_values, entry) == false) {
            current_id = entry;
            bitmap_set(used_ip_id_values, current_id);
        } else {
            entry = (entry + 3) % 65535;
        }
    } while (!current_id);
    iph->id = current_id;
    //mtx_unlock(&id_map_lock);

    return true;
}

/* Free IP ID value
 *
 * @param uint16_t ip id to free
 *
 * Note, I _think_ I don't need to lock the mutex, it shouldn't matter
 * that we're freeing this ID value whilst some other process is going on to
 * allocate a new ID for themselves.
 */
static inline void free_ipv4_id(uint16_t id) {
    bitmap_clear(used_ip_id_values, id);
}

/* Allocate IPv4 header when non-standard header is required.
 *
 * @param uint32_t src            -- Pointer to source sockaddr_in struct
 * @param uint32_t dst            -- Pointer to destination sockaddr_in struct
 * @param uint8_t tos             -- Type of service value
 * @param uint16_t f_off          -- Fragment offset
 * @param uint8_t ttl             -- Time to live
 * @param uint8_t proto           -- Protocol
 * @param uint8_t option_type     -- Type field for additional IPv4 options or 0 if unused
 * @param uint8_t option_len      -- Length of additional option octets or 0 if unused 
 * @param uint8_t *option_buf     -- Pointer to remaining `option_len` options or 0 if unused
 * @param uint16_t tlen           -- Amount of bytes in next protocol header
 *                                   and payload we're delivering
 * @return pointer to populated ipv4 header structure on success or NULL on error.
 * Errno is set for us by malloc()
 */
ipv4_hdr *create_ipv4_hdr(uint32_t src,
        uint32_t dst, uint8_t tos, uint16_t f_off_vcf,
        uint8_t ttl, uint8_t proto, uint8_t option_type, uint8_t option_len,
        uint8_t *option_buf, uint16_t tlen) 
{
    size_t len = sizeof(ipv4_hdr);
    if (option_type) {
        len++;
    }
    if (option_len) {
        len++;
        len += option_len;
    }
    if (len % 4) {
        len += ((len % 4) + (4 - (len % 4)));
    }
    uint8_t ihl = len / 4;

    ipv4_hdr *iph = calloc(1, sizeof(ipv4_hdr));
    if (!iph) {
        return iph;
    }

    iph->version = 4;
    iph->ihl = ihl;
    iph->tos = tos;
    iph->len = htons(len + tlen);
    iph->flags_foff = htons(f_off_vcf | (DONT_FRAGMENT << 8));
    iph->ttl = ttl;
    iph->ptcl = proto;
    iph->src = src;
    iph->dst = dst;
    iph->csum = 0;
    allocate_ipv4_id(iph);

    if (option_type) {
        memcpy(POINTER_ADD(void *, iph, sizeof(ipv4_hdr)), &option_type, 1);
    }
    if (option_len) {
        memcpy(POINTER_ADD(void *, iph, sizeof(ipv4_hdr) + 1), &option_len, 1);
        memcpy(POINTER_ADD(void *, iph, sizeof(ipv4_hdr) + 2), option_buf, option_len);
    }
    iph->csum = csum((uint16_t *)iph, len);

    return iph; 
}

/* Helper for parsing IPv4 Type of Service value 
 *
 * @param struct ipv4_socket_options *sopts -- Pointer to populated ipv4_socket_options structure
 * @return uint8_t type of service
 */
static inline uint8_t ipv4_parse_tos(ipv4_socket_options *sopts) {
    return (sopts->pre | (sopts->low_delay << 3) | (sopts->high_throughput << 4) | (sopts->high_reliability << 5));
}

/* Transmit datagram over IPv4 protocol
 *
 * @param net_socket *socket        -- Pointer to populated net_socket structure
 * @param uint32_t src   -- Pointer to populated source sockaddr_in structure
 * @param uint32_t dst   -- Pointer to populated destination sockaddr_in structure
 * @param const void *data          -- Pointer to datagram to send, including appropriate
 *                                     protocol header
 * @param size_t data_len           -- Length of datagram to send
 * @return amount of bytes sent excluding ip header on success or -1 on error.
 * Set errno on error.
 */
size_t ipv4_transmit_datagram(net_socket *socket, uint32_t src,
        uint32_t dst, const void *data, size_t data_len)
{
    ipv4_socket_options *iopts = (ipv4_socket_options *)socket->ip_options;
    uint8_t ttl = iopts->ttl;
//    uint8_t tos = ipv4_parse_tos(iopts);
    uint16_t f_off = 0 ? 2 : iopts->no_fragment;

    ipv4_hdr *ip_hdr = create_ipv4_hdr(src, dst, 0, f_off, ttl, socket->protocol, 
            0, 0, 0, data_len);
    if (!ip_hdr) {
        // Errno was set to us by malloc()
        return -1;
    }

    uint16_t size = ntohs(ip_hdr->len);
    void *packet = calloc(1, size);
    if (!packet) {
        // Errno was set to us by realloc()
        free(ip_hdr);
        return -1;
    }

    memcpy(packet, ip_hdr, sizeof(ipv4_hdr));
    memcpy(POINTER_ADD(void *, packet, 
                sizeof(ipv4_hdr)), data, data_len);

    // uint16_t sent = eth_transmit_frame(socket, (const void *)packet, size);
    uint16_t sent = link_tx(socket, (const void *)packet, size);

    free_ipv4_id(ip_hdr->id);
    free(ip_hdr);
    free(packet);
    return sent;
}

/* Receive datagram over IPv4 protocol
 *
 * @param net_socket *socket        -- Pointer to populated net_socket structure
 * @param const void *dst           -- Pointer to memory where we'll write received data to
 * @param size_t r_len              -- How many bytes do we want to receive *including* IPv4 header
 * @return size_t amount of bytes received on success or -1 on error.
 *                Set errno on error
 */
//size_t ipv4_receive_datagram(net_socket *socket, uint32_t caddr, void *dst, size_t r_len) 
//{
//    return ret;
//}



