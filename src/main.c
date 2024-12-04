/* Entrypoint for userspace network library
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include <errno.h>

#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "data_util.h"
#include "udp.h"
#include "ip.h"

int main(void) {
    struct sockaddr_in caddr;
    unsigned char c[64];

    net_socket *sock = new_socket(AF_INET, 17, "lan");
    if (!sock) {
        fprintf(stderr, "\nError: %d/%s\n", errno, strerror(errno));
        fflush(stderr);
        return -1;
    }
    printf("Got socket\n");
    ip_initialise();
    size_t sent = udp_send(sock, "172.20.239.160", "89.58.30.39", 1234, 1337, (uint8_t *)"Hello\n", 6);

    if (sent == -1) {
        printf("error: %d/%s\n", errno, strerror(errno));
    }

    return sent;
}

