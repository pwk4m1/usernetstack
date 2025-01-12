/* Entrypoint for userspace network library
 */

#include <sys/types.h>

#include <errno.h>

#include <stdio.h>
#include <string.h>

#include <socket.h>
#include <udp.h>
#include <ip.h>

//const char *TEST_SMAC = "\x56\x94\x9d\x02\x2e\x43";
const char *TEST_SMAC = "\xe0\x9d\x31\x29\x22\xe0";
const char *TEST_DMAC = "\xfa\x22\x23\x87\xa9\x9d"; 

int main(void) {
    net_socket *sock = new_socket(AF_INET, 17, (uint8_t*)TEST_SMAC, (uint8_t*)TEST_DMAC, "wlp2s0");
    if (!sock) {
        fprintf(stderr, "\nError: %d/%s\n", errno, strerror(errno));
        fflush(stderr);
        return -1;
    }
    ip_initialise();
    size_t sent = udp_send(sock, "172.20.239.149", "152.53.133.5", 1234, 1337, (uint8_t *)"Hellorld\n", 9);

    if (sent == -1) {
        printf("error: %d/%s\n", errno, strerror(errno));
    }

    return sent;
}


