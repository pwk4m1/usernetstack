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

/* Helpers and definitions to handle dealing with
 * different types of network links and link-layer
 * protocols we support.
 *
 */
#ifndef __NETLIB_LINK__
#define __NETLIB_LINK__

#include <sys/types.h>

#include <eth.h>
#include <socket.h>

/* different types of links we support
 *
 * @member ETH   -- Ethernet
 * @member SLIP  -- SLIP
 */
enum LINK_TYPE {
    ETH  = 0,
    SLIP = 1
};

/* Hold information related to link layer we're dealing with.
 *
 * @member enum LINK_TYPE type -- type of link we're communicating over
 * @member union hdr           -- pointer to link protocol specific data
 *
 */
typedef struct {
    enum LINK_TYPE type;
    union {
        eth_hdr *eth_header;
        uint16_t slip_port;
    } proto;
} link_options;

/* Transmit data over link that has been associated with this socket.
 *
 * @param net_socket *sock       -- Pointer to socket
 * @param const void *data       -- Pointer to data to trasmit
 * @param size_t size            -- Amount of bytes to send
 * @return size_t sent bytes.
 *         set errno on error.
 */
size_t link_tx(net_socket *sock, const void *data, size_t size);

#endif // __NETLIB_LINK__
