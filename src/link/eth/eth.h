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
#ifndef __UNET_ETH_H__
#define __UNET_ETH_H__

#include <stdint.h>

#include "../link.h"
#include "../../buffer/buffer.h"

enum ETH_TYPE {
    ETH_AF_INET = 0x0800
};

/**
 * Ethernet header structure.
 *
 * @param src Is the source MAC address
 * @param dst Is the destination MAC address
 * @param ptcl Is a protocol identifier word
 */
typedef struct {
    uint8_t src[6];
    uint8_t dst[6];
    uint16_t ptcl;
} ethernet_header;

/**
 * Data for ethernet link iface
 *
 */
typedef struct {
    uint16_t mtu;
    uint8_t src_mac[6];
} ethernet_link_data;

/**
 * Create a new ethernet link structure.
 *
 */
unet_link *create_eth_link(void *link_data);

/**
 * Transmit data over ethernet link.
 *
 * @param link Is a pointer to the link to use
 * @param packet Is a pointer to the packet buffer to transmit
 * @return amount of bytes sent
 */
uint16_t eth_tx(unet_link *link, buffer *packet);

/**
 * Receive a packet from ethernet link.
 *
 * @param link Is a pointer to the link to use
 * @param len Is the amount of bytes to read
 * @return a pointer to a new buffer containing the received data
 */
buffer *eth_rx(unet_link *link, uint16_t len);

#endif // __UNET_ETH_H__
