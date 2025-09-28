/*
 BSD 3-Clause License
 
 Copyright (c) 2025, k4m1  <me@k4m1.net>
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
#ifndef __UNET_IFACE_H__
#define __UNET_IFACE_H__

#include <sys/types.h>

#include <errno.h>
#include <stdint.h>

#include "../flexarr/flex.h"
#include "../link/link.h"

enum IFACE_STATE {
    up = 0,
    down,
    dormant,
    unknown
};

/**
 * This structure holds information about a given network interface,
 * and it's intended to be passed between different network protocol
 * layers in order to help with route & co. selection.
 *
 * @member name is an ascii name for the interface
 * @member state is the state the network intrafece is in, refer to
 *         enum IFACE_STATE
 * @member address_list Is a list of addresses associated with this
 *         interface
 * @member link is a pointer to link/layer2 and platform specific 
 *         information structure
 */
typedef struct {
    char *name;
    enum IFACE_STATE state;
    flexible_array *ipv4_address_list;
    flexible_array *ipv6_address_list;
    unet_link *link;
} net_interface;

/**
 * Create a new interface for the user.
 *
 */
net_interface *create_interface(char *name, unet_link *(*create_link_function)(void *data), void *link_data);

/**
 * Transmit a packet from a buffer over link-specific transmit
 * function associated with a given interface.
 *
 * @param iface Is a pointer to the interface to use
 * @param packet Is a pointer to the packet to transmit
 * @return amount of bytes received on success or -1 on error.
 *         Set errno on error.
 */
inline uint16_t iface_tx(net_interface *iface, buffer *packet) {
    if (iface->state == up) {
        return iface->link->tx(iface->link, packet);
    }
    errno = ENOLINK;
    return -1;
}

/**
 * Receive a packet from a given interface.
 *
 * @param iface Is a pointer to the interface to use
 * @param len Is the amount of bytes to receive
 * @return a pointer to a new buffer containing the received bytes or
 *         NULL on error.
 *         Set errno on error.
 */
inline buffer *iface_rx(net_interface *iface, uint16_t len) {
    if (iface->state == up) {
        return iface->link->rx(iface->link, len);
    }
    errno = ENOLINK;
    return NULL;
}

#endif // __UNET_IFACE_H__
