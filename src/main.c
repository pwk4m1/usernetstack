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

/* Entrypoint for userspace network library
 */

#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <data_util.h>
#include <socket.h>
#include <udp.h>
#include <ip.h>
#include <link.h>

//const char *TEST_SMAC = "\x56\x94\x9d\x02\x2e\x43";
const char *TEST_SMAC = "\xe0\x9d\x31\x29\x22\xe0";
const char *TEST_DMAC = "\xfa\x22\x23\x87\xa9\x9d"; 

int main(void) {
    net_socket *sock = new_socket(2, 17, SLIP, (uint8_t*)TEST_SMAC, (uint8_t*)TEST_DMAC, "wlp2s0");
    if (!sock) {
        fprintf(stderr, "\nError: %d/%s\n", errno, strerror(errno));
        fflush(stderr);
        return -1;
    }
    ip_initialise();
    uint32_t src_addr = inet_addr("10.0.0.2");
    uint32_t dst_addr = inet_addr("152.53.133.5");
    size_t sent = udp_send(sock, src_addr, dst_addr, 1234, 1337, (uint8_t *)"Hellorld\n", 9);

    if (sent == -1) {
        printf("error: %d/%s\n", errno, strerror(errno));
    }

    do {} while (1);
    return sent;
}


