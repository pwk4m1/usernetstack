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
struct iphdr *create_std_ipv4_hdr(struct sockaddr_in *src, 
        struct sockaddr *dst, uint8_t proto, uint16_t tlen);

#endif // __NETLIB_IP_H__
