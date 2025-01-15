/*
 BSD 3-Clause License
 
 Copyright (c) 2025, k4m1 <me@k4m1.net>
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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


