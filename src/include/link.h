/* Helpers and definitions to handle dealing with
 * different types of network links and link-layer
 * protocols we support.
 *
 */
#ifndef __NETLIB_LINK__
#define __NETLIB_LINK__

#include <sys/types.h>

#include <eth.h>
#include <socket.h>

/* different types of links we support
 *
 * @member ETH   -- Ethernet
 * @member SLIP  -- SLIP
 */
enum LINK_TYPE {
    ETH  = 0,
    SLIP = 1
};

/* Hold information related to link layer we're dealing with.
 *
 * @member enum LINK_TYPE type -- type of link we're communicating over
 * @member union hdr           -- pointer to link protocol specific data
 *
 */
typedef struct {
    enum LINK_TYPE type;
    union {
        eth_hdr *eth_header;
        uint16_t slip_port;
    } proto;
} link_options;

/* Transmit data over link that has been associated with this socket.
 *
 * @param net_socket *sock       -- Pointer to socket
 * @param const void *data       -- Pointer to data to trasmit
 * @param size_t size            -- Amount of bytes to send
 * @return size_t sent bytes.
 *         set errno on error.
 */
size_t link_tx(net_socket *sock, const void *data, size_t size);

#endif // __NETLIB_LINK__
