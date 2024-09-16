/* Helpers for creating IPv4 and IPv6 headers */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <assert.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <threads.h>

#include "bitmap.h"
#include "csum.h"

// This bitmask contains those ip id values that are
// currently in use.
static bitmap_t used_ip_id_values = 0;

// Last block we used for selecting ip id from
static uint16_t last_block = 0;

// maximum amout of blocks present
uint16_t max_blocks = 1024;

// Mutex lock for id bitmask
mtx_t id_map_lock;

/* Initialise ip header system */
void ip_initialise(void) {
    mtx_init(&id_map_lock, 0);
    srand(((uint64_t)&ip_initialise) % 0xffff);
    last_block = rand() % 0x1000; 
    used_ip_id_values = calloc(1, (65536 / 8));
    memset(used_ip_id_values, 0, (65536 / 8));
    assert(used_ip_id_values && "Could not allocate memory for ip id management");
}

/* Finalise ip header system */
void ip_finalise(void) {
    mtx_lock(&id_map_lock);
    mtx_destroy(&id_map_lock);
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
static bool allocate_ipv4_id(struct iphdr *iph) {
    bool stat = true;
    int mutex_result;

    /* Try and lock mutex, if result isn't success or busy,
     * we've errored out and should return false.
     */
    do {
        mutex_result = mtx_trylock(&id_map_lock);
    } while (mutex_result == thrd_busy);
    if (mutex_result != thrd_success) {
        return false;
    }

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
    mtx_unlock(&id_map_lock);

    return stat;
}

/* Allocate IPv4 header when non-standard header is required.
 *
 * @param struct sockaddr_in *src -- Pointer to source sockaddr_in struct
 * @param struct sockaddr_in *dst -- Pointer to destination sockaddr_in struct
 * @param uint8_t tos             -- Type of service value
 * @param uint16_t f_off_vcf      -- Fragment offset and control bits
 * @param uint8_t ttl             -- Time to live
 * @param uint8_t proto           -- Protocol
 * @param uint8_t option_type     -- Type field for additional IPv4 options or 0 if unused
 * @param uint8_t option_len      -- Length of additional option octets or 0 if unused 
 * @param uint8_t *option_buf     -- Pointer to remaining `option_len` options or 0 if unused
 * @param uint16_t tlen           -- Amount of bytes in next protocol header
 *                                   and payload we're delivering
 * @return pointer to populated ipv4 header structure
 */
struct iphdr *create_ipv4_hdr(struct sockaddr_in *src,
        struct sockaddr_in *dst, uint8_t tos, uint16_t f_off_vcf,
        uint8_t ttl, uint8_t proto, uint8_t option_type, uint8_t option_len,
        uint8_t *option_buf, uint16_t tlen) 
{
    size_t len = sizeof(struct iphdr);
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
    void *hdr = malloc(len);
    memset(hdr, 0, len);

    struct iphdr *iph = (struct iphdr *)hdr;
    iph->version = 4;
    iph->ihl = ihl;
    iph->tos = tos;
    iph->tot_len = htons(len + tlen);
    iph->frag_off = htons(f_off_vcf);
    iph->ttl = ttl;
    iph->protocol = proto;
    iph->saddr = src->sin_addr.s_addr;
    iph->daddr = dst->sin_addr.s_addr;
    iph->check = 0;
    allocate_ipv4_id(iph);

    if (option_type) {
        memcpy(hdr+sizeof(struct iphdr), &option_type, 1);
    }
    if (option_len) {
        memcpy(hdr+sizeof(struct iphdr)+1, &option_len, 1);
        memcpy(hdr+sizeof(struct iphdr)+2, option_buf, option_len);
    }
    iph->check = htons(csum((uint16_t *)hdr, 20));

    return hdr;
}

