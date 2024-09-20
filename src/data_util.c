/* Helper functions for dealing with various data and stuff :3 
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>

#include "data_util.h"

/* Internal helper for building sockaddr_in structure from given user input.
 *
 * @param char *addr        -- Pointer to ipv4 address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 * @return Pointer to a populated sockaddr_in structure
 */
struct sockaddr_in *build_sockaddr_in(char *addr, uint16_t port) {
    struct sockaddr_in *ret = calloc(1, sizeof(struct sockaddr_in));
    assert(ret);
    memset(ret, 0, sizeof(struct sockaddr_in));

    ret->sin_family = AF_INET;
    ret->sin_addr.s_addr = inet_addr((const char *)addr);
    ret->sin_port = htons(port);

    return ret;
}

/* Internal helper for building sockaddr_in6 structure from given user input.
 *
 * @param uint8_t *addr     -- Pointer to ipv4 address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 *
 */

/* Helper to populate sockaddr_in and sockaddr_in6 structures from
 * user provided data.
 *
 * @param int family        -- AF_INET or AF_INET6
 * @param char *addr        -- Pointer to ip address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 * @return pointer to populated struct sockaddr on success, or 0 if family
 *         isn't supported
 */
struct sockaddr *populate_sockaddr(int family, char *addr, uint16_t port) {
    struct sockaddr *ret = NULL;
    switch (family) {
        case (AF_INET):
            ret = (struct sockaddr *)build_sockaddr_in(addr, port);
            break;
        case (AF_INET6):
            ret = 0;
            errno = ENOSYS;
            break;
    }
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

