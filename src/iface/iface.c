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
#include <sys/types.h>

#include <stdint.h>
#include <stdlib.h>

#include "iface.h"
#include "../flexarr/flex.h"
#include "../link/link.h"

/**
 * Create a new interface for the user.
 *
 */
net_interface *create_interface(char *name, unet_link *(*create_link_function)(void *data), void *link_data) {
    net_interface *ret = calloc(1, sizeof(net_interface));
    if (!ret) {
        return NULL;
    }
    ret->name = name;
    ret->state = unknown;
    ret->ipv4_address_list = new_flex_array(sizeof(uint32_t), 1);
    ret->ipv6_address_list = new_flex_array((128 / sizeof(uint8_t)), 1);
    ret->link = create_link_function(link_data);

    if (!ret->link || !ret->ipv4_address_list || !ret->ipv6_address_list) {
        if (ret->link) {
            free(ret->link);
        }
        if (ret->ipv4_address_list) {
            free(ret->ipv4_address_list->data_array);
            free(ret->ipv4_address_list);
        }
        if (ret->ipv6_address_list) {
            free(ret->ipv6_address_list->data_array);
            free(ret->ipv6_address_list);
        }
        return NULL;
    }

    return ret;
}

