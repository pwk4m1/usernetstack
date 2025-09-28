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

#include <stdio.h>
#include <stdint.h>

#include "../../link.h"
#include "../../../buffer/buffer.h"

typedef struct {
    char *name;
    uint8_t mac[6];
} linux_eth_data;

uint16_t eth_tx(unet_link *link, buffer *packet) {
    printf("ETH Link (%p) tx %lx bytes\n", link, packet->len);
    return packet->len;
}

buffer *eth_rx(unet_link *link, uint16_t len) {
    printf("ETH Link (%p) rx for %x bytes requested\n", link, len);
    buffer *ret = new_buffer(len);
    return ret;
}

unet_link *create_eth_link(void *link_data) {
    char *name = (char *)link_data;
    unet_link *ret = calloc(1, sizeof(unet_link));
    if (!ret) {
        return NULL;
    }
    ret->mtu = 1500;
    ret->rx = eth_rx;
    ret->tx = eth_tx;
    ret->ptcl_data = (linux_eth_data *)calloc(1, sizeof(linux_eth_data));
    if (!ret->ptcl_data) {
        free(ret);
        return NULL;
    }
    linux_eth_data *data = (linux_eth_data *)ret->ptcl_data;
    data->name = name;
    memset(data->mac, 0, 6);
    return ret;
}

