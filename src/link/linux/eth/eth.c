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

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#include <stdio.h>
#include <stdint.h>

#include <unistd.h>

#include "../../eth/eth.h"
#include "../../link.h"
#include "../../../buffer/buffer.h"

static const char *MAC_SRC = "\xe0\x9d\x31\x29\x22\xe0";

typedef struct {
    char *name;
    int sock;
    struct sockaddr_ll saddr;
    uint8_t mac[6];
} linux_eth_data;

/**
 * Temporary helper to write higher-level parts first
 * before moving on to low-level NIC handling and stuff
 *
 */
static int open_socket(char *iface_name) {
    int sock = socket(AF_PACKET, SOCK_RAW, htons(3));
    if (sock == -1) {
        return -1;
    }
    int stat = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, iface_name, strlen(iface_name));
    if (stat == -1) {
        close(sock);
        return -1;
    }
    return sock;
}


uint16_t eth_tx(unet_link *link, buffer *packet) {
    printf("ETH Link (%p) tx %lx bytes\n", link, packet->len);
    linux_eth_data *data = (linux_eth_data *)link->ptcl_data;

    return sendto(data->sock, packet->buf, packet->len, 0,
            (const struct sockaddr *)&data->saddr, sizeof(struct sockaddr_ll));
    //return packet->len;
}

buffer *eth_rx(unet_link *link, uint16_t len) {
    printf("ETH Link (%p) rx for %x bytes requested\n", link, len);
    len += sizeof(ethernet_header);
    buffer *ret = new_buffer(len);
    linux_eth_data *data = (linux_eth_data *)link->ptcl_data;
    socklen_t slen = sizeof(struct sockaddr_ll);
    ssize_t got = recvfrom(data->sock, ret->buf, ret->len, 0, 
            (struct sockaddr *)&data->saddr, &slen);
    if (got != len) {
        ret = resize_buffer(ret, got);
    }
    for (int i = 0; i < len; i++) {
        printf("%02x ", ret->buf[i]);
    }
    printf("\n");
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
    data->sock = open_socket(name);
    if (data->sock == -1) {
        free(ret);
        return NULL;
    }
    memcpy(data->saddr.sll_addr, MAC_SRC, 6);
    data->saddr.sll_family = AF_PACKET;
    data->saddr.sll_protocol = htons(ETH_P_ALL);
    data->saddr.sll_ifindex = if_nametoindex(name);
    data->saddr.sll_hatype = 1;
    data->saddr.sll_pkttype = PACKET_OTHERHOST;
    data->saddr.sll_halen = ETH_ALEN;
    data->name = name;
    memcpy(data->mac, MAC_SRC, 6);
    return ret;
}

