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
#ifndef __UNET_ROUTE_H__
#define __UNET_ROUTE_H__

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "../iface/iface.h"

/**
 * This structure holds information related to a given network route,
 * and is used by IP layer to select which interface/link/address to
 * use for transmiting the datagram.
 *
 * This routing table structure is intended to be used with devices
 * that have big(tm) routing tables.
 *
 */
typedef struct {
    uint16_t level1[65536];
    uint16_t level2[65536];

    net_interface *iface_arrray;
} huge_ipv4_route_table;

static inline huge_ipv4_route_table *new_huge_route_table(void) {
    // TODO
    errno = ENOSYS;
    return NULL;
}

/**
 * This structure holds information of ipv4 route entries for
 * small routing tables.
 *
 * @member default_route Is set to true if this route points to a router 
 * @member network Is the network this route is associated with
 * @member mask Is the mask to apply for checking if we have route
 *         to the address
 * @member gateway Is the full gateway address
 * @member net_interface *iface Is the network interface to use
 */
typedef struct {
    bool default_route;
    uint32_t network;
    uint32_t mask;
    uint32_t gateway;
    uint32_t address;
    net_interface *iface;
} ipv4_route_entry;

/**
 * This structure is to be used for routing on devices with
 * just a handful of routes, such as home PCs, some servers, etc.
 *
 * @member route Is an array of routes we have
 * @member count Is the amount of routes currently present
 */
typedef struct {
    ipv4_route_entry *entry_array[256];
    uint64_t count;
} small_ipv4_route_table;

/**
 * Generic helper structure for dealing with all IPv4 routing tables
 *
 */
typedef struct {
    bool use_small_table;
    union {
        huge_ipv4_route_table *huge;
        small_ipv4_route_table *small;
    } pointers;
} ipv4_routing_table;

/**
 * Helper to allocate space for a new IPv4 routing table
 *
 */
static inline ipv4_routing_table *new_ipv4_route_table(void) {
    ipv4_routing_table *ret = calloc(1, sizeof(ipv4_routing_table));
    if (ret) {
        ret->use_small_table = true;
        ret->pointers.small = calloc(1, sizeof(small_ipv4_route_table));
        if (!ret->pointers.small) {
            free(ret);
            ret = NULL;
        }
    }
    return ret;
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
        uint32_t mask, uint32_t gateway, net_interface *iface);


/**
 * Helper to get a route to a given ipv4 host 
 *
 * @param address Is the IPv4 destination address to connect to
 * @return pointer to appropriate route structure on success or
 *         NULL on error. Set errno on error.
 */
ipv4_route_entry *ipv4_get_route_to_host(uint32_t address);

#endif // __UNET_ROUTE_H__
