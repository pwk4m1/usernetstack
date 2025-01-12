/* Implement ip/{tcp,udp} checksum calculation */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include <csum.h>

/* Craft a ipv4 pseudo header from source/destination sockaddr_in structures
 *
 * @param struct sockaddr_in *src -- Pointer source sockaddr_in
 * @param struct sockaddr_in *dst -- Pointer to destination sockaddr_in
 * @param uint8_t ptcl            -- Protocol identifier
 * @param size_t len              -- Size of payload to transmit
 * @return pointer to populated ipv4_psd_hdr
 */
ipv4_psd_hdr *craft_ipv4_psd_hdr(struct sockaddr_in *src,
        struct sockaddr_in *dst, uint8_t ptcl,
        size_t len)
{
    ipv4_psd_hdr *ret = calloc(1, sizeof(ipv4_psd_hdr));
    assert(ret && "Unable to allocate memory for pseudo header\n");

    ret->src = src->sin_addr.s_addr;
    ret->dst = dst->sin_addr.s_addr;
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

