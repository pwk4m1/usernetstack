/* Helpers for our socket operations
 *
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#include <string.h>

#include <unistd.h>

#include <data_util.h>
#include <ip.h>
#include <link.h>
#include <socket.h>

/* Get and setup unix-styled socket for us
 *
 */
int raw_socket(const char *iface) {
    int sock = socket(AF_PACKET, SOCK_RAW, htons(3));
    if (sock == -1) {
        return sock;
    }
    int stat = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, iface, strlen(iface));
    if (stat == -1) {
        close(sock);
        return stat;
    }
    return sock;
}

/* Send up to size_t bytes of data
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to data to transmit
 * @param size_t len       -- Amount of bytes to send
 * @return amount of bytes written on success or -1 on error.
 *         set errno on error.
 */
size_t transmit(net_socket *sock, const void *data, size_t len) {
    struct sockaddr_ll saddr;
    link_options *link = (link_options *)sock->link_options;

    memcpy(saddr.sll_addr, link->src_mac, 6);
    saddr.sll_family   = AF_PACKET;
    saddr.sll_protocol = htons(ETH_P_ALL);
    saddr.sll_ifindex  = if_nametoindex(sock->iface);
    saddr.sll_hatype   = 1;
    saddr.sll_pkttype  = PACKET_OTHERHOST;
    saddr.sll_halen    = ETH_ALEN;


    return sendto(sock->raw_sockfd, data, len, 0, 
            (const struct sockaddr *)&saddr, sizeof(struct sockaddr_ll));
}


