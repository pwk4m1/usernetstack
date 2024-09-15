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

#include "udp.h"
#include "ip.h"

int main(void) {
    ip_initialise();

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (!sock) {
        fprintf(stderr, "Unable to open raw socket\n");
        return -1;
    }
    char *msg = "Hello world\n";

    for (int i = 0; i < 1; i++) {
        size_t stat = udp_send(sock, AF_INET, "172.20.239.157", "89.58.30.39", 1234, 1337,
                (uint8_t *)msg, strlen(msg));

        if (stat == (size_t)-1) {
            printf("Sending upd packet failed: %d/%s\n", errno, strerror(errno));
        } else {
            printf("Packet sent successfully\n");
        }
    }

    ip_finalise();
    return 0;

}

