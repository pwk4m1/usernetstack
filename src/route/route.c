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

#include <errno.h>
#include <stdint.h>

#include "route.h"

/**
 * Pointer to the currently used ipv4 routing table
 *
 */
extern ipv4_routing_table *table;

/**
 * Helper to get the appropriate/currently used IPv4 routing table content.
 *
 */
static inline void *get_current_table(void) {
    if (table->use_small_table) {
        return (void *)table->pointers.small;
    } else {
        errno = ENOSYS;
        return NULL;
    }
}

/**
 * Add a new entry for small ipv4 routing table
 *
 * @param default_route Indicates that this is the default route to use
 * @param network Is the network this route is associated with
 * @param mask Is the mask to apply for checking if we have route
 *         to the address
 * @param gateway Is the full gateway address
 * @param net_interface *iface Is the network interface to use
 * @return Pointer to new entry on success or NULL on error.
 */
ipv4_route_entry *add_ipv4_route_entry(bool default_route, uint32_t network,
        uint32_t mask, uint32_t gateway, net_interface *iface) {
    small_ipv4_route_table *table = get_current_table();
    if (!table) {
        return NULL;
    }
    uint64_t off = table->count;
    ipv4_route_entry *entry = calloc(1, sizeof(ipv4_route_entry));
    if (!entry) {
        return NULL;
    }
    entry->address = network;
    entry->mask = mask;
    entry->gateway = gateway;
    entry->iface = iface;
    entry->default_route = default_route;
    table->entry_array[off] = entry;
    table->count++;

    return entry;
}

/**
 * Get interface for a given destination address
 *
 * @param address Is the IPv4 address to get interface/route for
 * @return pointer to the associated route entry on success or NULL on error
 *         Set errno on error.
 */
static ipv4_route_entry *get_small_table_inet_entry(small_ipv4_route_table *table, uint32_t address) {
    for (uint64_t off = 0; off < table->count; off++) {
        uint32_t current = (address & ~(table->entry_array[off]->mask));
        if (current == (table->entry_array[off]->network & ~(table->entry_array[off]->mask))) {
            return table->entry_array[off];
        }
    }
    for (uint64_t off = 0; off < table->count; off++) {
        if (table->entry_array[off]->default_route) {
            return table->entry_array[off];
        }
    }
    errno = EHOSTUNREACH;
    return NULL;
}

/**
 * Helper to get route to a host
 *
 * @param address Is the ipv4 host we want to find a route to
 * @return route structure for if match is found, NULL otherwise.
 *         Set errno on error. 
 */
ipv4_route_entry *ipv4_get_route_to_host(uint32_t address) {
    if (table->use_small_table == false) {
        errno = ENOSYS;
        return NULL;
    }
    small_ipv4_route_table *table = get_current_table();
    if (!table) {
        return NULL;
    }
    return get_small_table_inet_entry(table, address);
}


