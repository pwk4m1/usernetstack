/* Send up to size_t bytes of data
 *
 * @param net_socket *sock -- Pointer to socket we're working with
 * @param const void *data -- Pointer to data to transmit
 * @param size_t len       -- Amount of bytes to send
 * @return amount of bytes written on success or -1 on error.
 *         set errno on error.
 */
#include <sys/syscalls.h>
#include <sys/io.h>

#include <socket.h>
#include <link.h>
#include <slip.h>

/* Open a raw network socket for user
 *
 * @param const char *iface -- Name of interface to use
 * @return int socket on success or -1 on error.
 */
int raw_socket(const char *iface) {
    uint64_t err;
    sc_do_hardware_ioperm(0x2f8, 7, true, &err);
    return err;
}

size_t transmit(net_socket *sock, const void *data, size_t len) {
/*    uint64_t err;
    sc_do_hardware_ioperm(0x3f8, 7, true, &err);
    link_options *link = (link_options *)sock->link_options;
    */
    return 0;
}

