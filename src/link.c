/* Helpers and definitions to handle dealing with
 * different types of network links and link-layer
 * protocols we support.
 *
 */

#include <sys/types.h>

#include <eth.h>

#include <link.h>
#include <socket.h>

/* Transmit data over link that has been associated with this socket.
 *
 * @param net_socket *sock -- Pointer to our network socket
 * @param const void *data -- Pointer to data to trasmit
 * @param size_t size      -- Amount of bytes to send
 * @return size_t sent bytes.
 *         set errno on error.
 */
size_t link_tx(net_socket *sock, const void *data, size_t size) {
    link_options *link = (link_options *)sock->link_options;
    size_t ret = 0;

    switch (link->type) {
    case (ETH):
        ret = eth_transmit(sock, data, size);
        break;
    case (SLIP):
        // TODO
        //
        break;
    default:
        break;
    }
    return ret;

}


