/* Helpers for creating IPv4 and IPv6 headers */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <netpacket/packet.h>
#include <net/ethernet.h>

#include <assert.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include <stdio.h>

#include <threads.h>

#include "eth.h"
#include "ip.h"
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
 * @return pointer to populated ipv4 header structure on success or NULL on error.
 * Errno is set for us by malloc()
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
    if (!hdr) {
        return 0;
    }
    memset(hdr, 0, len);

    struct iphdr *iph = (struct iphdr *)hdr;
    iph->version = 4;
    iph->ihl = ihl;
    iph->tos = tos;
    iph->tot_len = htons(len + tlen);
    iph->frag_off = f_off_vcf;
    iph->ttl = ttl;
    iph->protocol = proto;
    iph->saddr = src->sin_addr.s_addr;
    iph->daddr = dst->sin_addr.s_addr;
    iph->check = 0;
    allocate_ipv4_id(iph);

    if (option_type) {
        memcpy(POINTER_ADD(void *, hdr, sizeof(struct iphdr)), &option_type, 1);
    }
    if (option_len) {
        memcpy(POINTER_ADD(void *, hdr, sizeof(struct iphdr) + 1), &option_len, 1);
        memcpy(POINTER_ADD(void *, hdr, sizeof(struct iphdr) + 2), option_buf, option_len);
    }
    iph->check = csum((uint16_t *)hdr, len);

    return hdr;
}

/* Helper for parsing IPv4 Type of Service value 
 *
 * @param struct ipv4_socket_options *sopts -- Pointer to populated ipv4_socket_options structure
 * @return uint8_t type of service
 */
static inline uint8_t ipv4_parse_tos(struct ipv4_socket_options *sopts) {
    return (sopts->pre | (sopts->low_delay << 3) | (sopts->high_throughput << 4) | (sopts->high_reliability << 5));
}

static inline void log_pkt(void *packet, size_t size) {
    FILE *f = fopen("log.bin", "wb");
    fwrite(packet, size, 1, f);
    fclose(f);
}

/* Transmit datagram over IPv4 protocol
 *
 * @param net_socket *socket        -- Pointer to populated net_socket structure
 * @param struct sockaddr_in *src   -- Pointer to populated source sockaddr_in structure
 * @param struct sockaddr_in *dst   -- Pointer to populated destination sockaddr_in structure
 * @param const void *data          -- Pointer to datagram to send, including appropriate
 *                                     protocol header
 * @param size_t data_len           -- Length of datagram to send
 * @return amount of bytes sent excluding ip header on success or -1 on error.
 * Set errno on error.
 */
size_t ipv4_transmit_datagram(net_socket *socket, struct sockaddr_in *src,
        struct sockaddr_in *dst, const void *data, size_t data_len)
{
    struct ipv4_socket_options *iopts = (struct ipv4_socket_options *)socket->ip_options;
    uint8_t ttl = iopts->ttl;
    uint8_t tos = ipv4_parse_tos(iopts);
    uint16_t f_off_vcf = 0 ? 2 : iopts->no_fragment;

    struct ethhdr *eth_hdr = create_eth_hdr(socket, ETH_P_IP);
    struct iphdr *ip_hdr = create_ipv4_hdr(src, dst, tos, f_off_vcf, ttl, socket->protocol, 
            0, 0, 0, data_len);
    if (!ip_hdr) {
        // Errno was set to us by malloc()
        return -1;
    }

    uint16_t size = ntohs(ip_hdr->tot_len) + sizeof(struct ethhdr);
    printf("size: %d\n", size);
    void *packet = calloc(1, size);
    if (!packet) {
        // Errno was set to us by realloc()
        free(ip_hdr);
        return -1;
    }
    memcpy(packet, eth_hdr, sizeof(struct ethhdr));
    memcpy(POINTER_ADD(void *, packet, sizeof(struct ethhdr)), 
            ip_hdr, sizeof(struct iphdr));

    memcpy(POINTER_ADD(void *, packet, 
                (sizeof(struct ethhdr) + (sizeof(struct iphdr)))), data, data_len);

    printf("Calling sendto()\n");
    log_pkt(packet, size);
    size_t written = sendto(socket->raw_sockfd, packet, size, 0, 
            (struct sockaddr *)socket->link_saddr, sizeof(struct sockaddr_ll));
    // TODO: ICMP Error checks

    free_ipv4_id(ip_hdr->id);
    free(packet);

    return written;
}

/* Receive datagram over IPv4 protocol
 *
 * @param net_socket *socket        -- Pointer to populated net_socket structure
 * @param const void *dst           -- Pointer to memory where we'll write received data to
 * @param size_t r_len              -- How many bytes do we want to receive *including* IPv4 header
 * @return size_t amount of bytes received on success or -1 on error.
 *                Set errno on error
 */
//size_t ipv4_receive_datagram(net_socket *socket, struct sockaddr_in *caddr, void *dst, size_t r_len) 
//{
//    return ret;
//}





