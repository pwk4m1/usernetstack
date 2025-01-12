/* Helpers and declarations for Serial Line Internet Protocol (SLIP) 
 * implementation.
 */
#ifndef __NETLIB_SLIP__
#define __NETLIB_SLIP__

#include <sys/types.h>
#include <stdint.h>

static const unsigned char SLIP_FRAME_END = 0xC0;
static const unsigned char SLIP_FRAME_ESCAPE = 0xDB;
static const unsigned char SLIP_ESCAPE_END = 0xDC;
static const unsigned char SLIP_ESCAPE_ESCAPE = 0xDD;

/* Transmit packet over slip
 *
 * @param uint16_t port    -- Port to use
 * @param const void *data -- Pointer to data to send
 * @param size_t len       -- Amount of bytes to write
 */
size_t slip_transmit(uint16_t port, const void *data, size_t len);

#endif // __NETLIB_SLIP__
