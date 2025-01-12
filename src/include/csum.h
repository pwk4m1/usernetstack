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
