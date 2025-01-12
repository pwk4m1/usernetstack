/* Serial Line Internet Protocol (SLIP) implementation
 *
 */
#include <sys/types.h>
#include <sys/io.h>

#include <stdint.h>

#include <slip.h>

static inline void serial_wait(uint16_t port) {
    do { } while ((inb(port + 5) & 0x20) == 0);
}

static inline void tx_byte(unsigned char c, uint16_t port) {
    serial_wait(port);
    outb(port, c);
}

/* Transmit packet over slip
 *
 * @param uint16_t port    -- Port to use
 * @param const void *data -- Pointer to data to send
 * @param size_t len       -- Amount of bytes to write
 */
size_t slip_transmit(uint16_t port, const void *data, size_t len) {
    unsigned char *tx = (unsigned char *)data;
    tx_byte(SLIP_FRAME_END, port);
    
    for (size_t i = 0; i < len; i++) {
        unsigned char c = tx[i];
        switch (c) {
        case (SLIP_FRAME_END):
            tx_byte(SLIP_ESCAPE_END, port);
            break;
        case (SLIP_FRAME_ESCAPE):
            tx_byte(SLIP_ESCAPE_ESCAPE, port);
            break;
        }
        tx_byte(c, port);
    }
    tx_byte(SLIP_FRAME_END, port);

    return len;
}


