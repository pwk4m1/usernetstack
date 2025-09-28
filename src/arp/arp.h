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
#ifndef __UNET_ARP_H__
#define __UNET_ARP_H__

#include <stdbool.h>
#include <stdint.h>

#include "../iface/iface.h"
#include "../llist/llist.h"

enum ARP_OPCODE {
    REQUEST  = 0x0001,
    RESPONSE = 0x0002
};

/**
 * ARP cache table
 *
 * @member iface Is the interface associated with this entry
 * @member hardware_type Specifies the network link protocol type
 * @member protocol_type Specifies the internetwork protocol type
 * @member hw_address Is the hardware address of the remote peer
 * @member ptcl_address Is the internetwork protocol address of the remote peer
 * @member reachable Is set if we can transmit bits to the remote peer
 * @member permanent Is set if this is a static entry
 * @member ttl Is the time-to-live for non-permanent entry
 */
typedef struct __attribute__((packed)) {
    net_interface *iface;
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hw_address[6];
    uint32_t ptcl_address;
    bool reachable;
    bool permanent;
    uint16_t ttl;
} arp_cache_table_entry;

/**
 * Data structure for address resolution protocol packets.
 *
 * @member hardware_type Specifies the network link protocol type
 * @member protocol_type Specifies the internetwork protocol type
 * @member hardware_size Is length of the hardware address
 * @member protocol_size Is lenght of the protocol address
 * @member operation Differentiates between request/response
 * @member source_hw_address Is the sender hardware address
 * @member source_ptcl_address Is the internetwork protocol address of sender
 * @member target_hw_address Is the receiver hardware address
 * @member target_ptcl_address Is the internetwork protocol address of intended receiver
 */
typedef struct __attribute__((packed)) {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t  hardware_size;
    uint8_t  protocol_size;
    enum ARP_OPCODE operation : 16;
    uint8_t source_hw_address[6];
    uint32_t source_ptcl_address;
    uint8_t target_hw_address[6];
    uint32_t target_ptcl_address;
} arp_packet;

/**
 * Helper for allocating new arp cache table entry
 *
 * @return A pointer to new entry structure on success or NULL on error
 */
static inline arp_cache_table_entry *new_arp_table_entry(void) {
    return (arp_cache_table_entry *)calloc(1, sizeof(arp_cache_table_entry));
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
        bool reachable, bool permanent, uint16_t ttl);

/**
 * Remove an entry from the arp cache table.
 *
 * @param entry Is the entry to remove
 */
static inline void arp_remove_entry(linked_list *table, arp_cache_table_entry *entry) {
    ll_del_entry(table, (void *)entry);
}

/**
 * Helper for allocating new arp packet 
 *
 * @return A pointer to new packet on success or NULL on error
 */
static inline arp_packet *new_arp_packet(void) {
    return (arp_packet *)calloc(1, sizeof(arp_packet));
}

/**
 * Broadcast on a network to find neighbours/peers
 *
 * @param table Is a pointer to allocated arp table
 * @param iface Is the network interface to work with
 * @return uint64_t amount of neighbours found
 */
uint64_t arp_find_neighbours(linked_list *table, net_interface *iface);

#endif // __UNET_ARP_H__
