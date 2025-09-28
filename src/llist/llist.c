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
#include <stdint.h>
#include <stdlib.h>

#include "llist.h"

/**
 * Helper for allocating new linked list entries.
 *
 * @return A pointer to the new entry on success or NULL on error.
 */
static ll_entry *new_entry(void) {
    return (ll_entry *)calloc(1, sizeof(ll_entry));
}

/**
 * Add a new entry to an existing linked list
 *
 * @param list Is the list to add the member to
 * @param data Is data associated with this entry
 * @return A pointer to the new entry on success or NULL on error.
 */
ll_entry *ll_add_entry(linked_list *list, void *data) {
    ll_entry *ret = new_entry();
    if (ret) {
        ret->data = data;
        list->size++;
        if (list->first == NULL) {
            list->first = ret;
            list->last = ret;
        } else {
            ret->previous = list->last;
            ret->next = NULL;
            list->last->next = ret;
            list->last = ret;
        }
    }
    return ret;
}

/**
 * Search for a entry that contains given data in the given
 * linked list.
 *
 * @param list Is the list to search the member from 
 * @param data Is data associated with entry to find
 * @return A pointer to the entry on success or NULL on error.
 */
ll_entry *ll_get_entry_by_data(linked_list *list, void *data) {
    if (list->first == NULL) {
        return NULL;
    }
    ll_entry *e = list->first;
    while (e != NULL) {
        if (e->data == data) {
            return e;
        }
        e = e->next;
    }
    return NULL;
}

/**
 * Search for a Nth entry in the list.
 *
 * @param list Is the list to search the member from 
 * @param offset Is the node-number to find
 * @return A pointer to the entry on success or NULL on error.
 */
ll_entry *ll_get_entry_by_offset(linked_list *list, uint64_t offset) {
    if ((list->first == NULL) || (list->size < offset)) {
        return NULL;
    }
    ll_entry *ret = list->first;
    for (uint64_t o = 0; o < offset; o++) {
        ret = ret->next;
    }
    return ret;
}

/**
 * Delete a given entry from the given list.
 *
 * @param list Is the list to delete member from
 * @param entry Is the entry to delete
 */
void ll_del_entry(linked_list *list, ll_entry *entry) {
    if (list->first) {
        ll_entry *e = list->first;
        while (e && e != entry) {
            e = e->next;
        }
        if (e) {
            if (e->previous) {
                e->previous->next = e->next;
            }
            if (e->next) {
                e->next->previous = e->previous;
            }
            free(e->data);
            free(e);
        }
        list->size--;
    }
}

/**
 * Delete a pre-existing linked list, freeing all the entries
 * as well as the associated data.
 */
void delete_linked_list(linked_list *list) {
    while (list->size > 0) {
        ll_del_entry(list, list->last);
    }
    free(list);
}

