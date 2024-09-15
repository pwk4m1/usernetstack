/* Helpers for createing IPv4 and IPv6 headers */
#ifndef __NETLIB_IP_H__
#define __NETLIB_IP_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <stdint.h>

/* Initialise ip header system */
void ip_initialise(void);

/* Finalise ip header system */
void ip_finalise(void);

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
        uint8_t *option_buf, uint16_t tlen);


/* Allocate and populate a default non-priority IPv4 header for user.
 * Unless there's a need for high priority delay, reliability or throughput, this
 * is likely the type of routine packet header you want.
 *
 * @param struct sockaddr_in *src -- Pointer to source sockaddr_in struct
 * @param struct sockaddr_in *dst -- Pointer to destination sockaddr_in struct
 * @param uint8_t proto           -- Protocol identification number
 * @param uint16_t tlen           -- Amount of bytes in next protocol header
 *                                   and payload we're delivering
 * @return pointer to populated ipv4 header structure
 */
inline struct iphdr *create_std_ipv4_hdr(struct sockaddr_in *src, 
        struct sockaddr_in *dst, uint8_t proto, uint16_t tlen) {
    return create_ipv4_hdr(src, dst, 16, 0, 64, proto, 0, 0, 0, tlen);
}

#endif // __NETLIB_IP_H__
