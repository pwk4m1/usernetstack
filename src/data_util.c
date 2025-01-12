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

