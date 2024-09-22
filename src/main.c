/* Entrypoint for userspace network library
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include <stdio.h>

#include <unistd.h>

#include "data_util.h"
#include "udp.h"
#include "ip.h"

int main(void) {
    net_socket *sock = new_socket(AF_INET, 17);
    if (!sock) {
        fprintf(stderr, "Unable to obtain network socket\n");
        return -1;
    }
    ip_initialise();
    size_t sent = udp_send(sock, "172.20.239.160", "89.58.30.39", 1234, 1337, (uint8_t *)"Hello\n", 6);
    return sent;
}

