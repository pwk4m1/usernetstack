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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "arp.h"
#include "../iface/iface.h"
#include "../link/eth/eth.h"
#include "../llist/llist.h"

/**
 * Helper to get appropriate hardware size for given type
 *
 * @param type Is the hardware type to get size for
 * @return size for the hardware type
 */
static uint16_t get_hw_size(uint16_t type) {
    switch (type) {
    default:
        return 6;
    }
}

/**
 * Add a new entry to the arp cache table
 *
 * @param cache Is a pointer to the arp cache table
 * @param iface Is the interface associated with this entry
 * @param hardware_type Specifies the network link protocol type
 * @param protocol_type Specifies the internetwork protocol type
 * @param hw_address Is the hardware address of the remote peer
 * @param ptcl_address Is the internetwork protocol address of the remote peer
 * @param reachable Is set if we can transmit bits to the remote peer
 * @param permanent Is set if this is a static entry
 * @param ttl Is the time-to-live for non-permanent entry
 * @return A pointer to new entry structure on success or NULL on error
 */
arp_cache_table_entry *add_arp_table_entry(linked_list *cache, net_interface *iface,
        uint16_t hardware_type, uint16_t protocol_type,
        uint8_t *hw_address, uint32_t ptcl_address,
        bool reachable, bool permanent, uint16_t ttl)
{
    arp_cache_table_entry *entry = new_arp_table_entry();
    if (!entry) {
        return NULL;
    }
    entry->iface = iface;
    entry->hardware_type = hardware_type;
    entry->protocol_type = protocol_type;
    memcpy(entry->hw_address, hw_address, get_hw_size(hardware_type));
    entry->ptcl_address = ptcl_address;
    entry->reachable = reachable;
    entry->permanent = permanent;
    entry->ttl = ttl;
    ll_add_entry(cache, (void *)entry);
    return entry;
}

/**
 * Helper to build broadcast packet for IPv4 for a given interface
 *
 * @param iface Is the network interface to work with
 * @return a pointer to populated arp_packet structure
 */
static buffer *new_arp_bc_packet(net_interface *iface) {
    buffer *ret = new_buffer(sizeof(arp_packet));
    if (!ret) {
        return NULL;
    }
    ethernet_link_data *eth_data = (ethernet_link_data *)iface->link->ptcl_data;
    arp_packet *pkt = (arp_packet *)ret->buf;
    pkt->hardware_type = iface->link->type;
    pkt->protocol_type = ETH_AF_INET;
    pkt->hardware_size = get_hw_size(pkt->hardware_type);
    pkt->protocol_size = sizeof(uint32_t);
    pkt->operation = REQUEST;
    memcpy(pkt->source_hw_address, eth_data->src_mac, sizeof(eth_data->src_mac));
    pkt->source_ptcl_address = (uint32_t)(*(uint32_t *)farr_get_entry(iface->ipv4_address_list, 0));
    memset(pkt->target_hw_address, 0, sizeof(pkt->target_hw_address));
    pkt->target_ptcl_address = 0;
    return ret; 
}


/**
 * Broadcast on a network to find neighbours/peers
 *
 * @param table Is a pointer to allocated arp table
 * @param iface Is the network interface to work with
 * @return uint64_t amount of neighbours found.
 *         Set errno on error.
 */
uint64_t arp_find_neighbours(linked_list *table, net_interface *iface) {
    buffer *pkt = new_arp_bc_packet(iface);
    if (!pkt) {
        return 0;
    }
    iface_tx(iface, pkt);
    free(pkt);
    buffer *response = iface_rx(iface, sizeof(arp_packet));
    if (!response) {
        return 0;
    }
    arp_packet *response_fields = (arp_packet *)response->buf;
    if (response_fields->operation != RESPONSE) {
        free(response);
        errno = EPROTO;
        return 0;
    }
    add_arp_table_entry(table, iface, response_fields->hardware_type,
            response_fields->protocol_type, response_fields->source_hw_address,
            response_fields->source_ptcl_address, true, false, 30);
    free(response);
    return 1;
}

