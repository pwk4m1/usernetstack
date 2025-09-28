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
#ifndef __UNET_LLIST_H__
#define __UNET_LLIST_H__

#include <stdint.h>
#include <stdlib.h>

/**
 * Entry of a linked list.
 *
 * @member previous Is a pointer to the previous entry in this list
 * @member next Is a pointer to the next entry in this list
 * @member data Is a pointer to the data associated with this entry
 */
typedef struct ll_entry {
    struct ll_entry *previous;
    struct ll_entry *next;
    void *data;
} ll_entry;

/**
 * Linked list structure.
 *
 * @member first Is a pointer to the 1st entry in this list
 * @member last Is a pointer to the last entry in this list
 * @member size Is the amount of entries present in this list
 *
 */
typedef struct linked_list {
    ll_entry *first;
    ll_entry *last;
    uint64_t size;
} linked_list;

/**
 * Add a new entry to an existing linked list
 *
 * @param list Is the list to add the member to
 * @param data Is data associated with this entry
 * @return A pointer to the new entry on success or NULL on error.
 */
ll_entry *ll_add_entry(linked_list *list, void *data);

/**
 * Search for a entry that contains given data in the given
 * linked list.
 *
 * @param list Is the list to search the member from 
 * @param data Is data associated with entry to find
 * @return A pointer to the entry on success or NULL on error.
 */
ll_entry *ll_get_entry_by_data(linked_list *list, void *data);

/**
 * Search for a Nth entry in the list.
 *
 * @param list Is the list to search the member from 
 * @param offset Is the node-number to find
 * @return A pointer to the entry on success or NULL on error.
 */
ll_entry *ll_get_entry_by_offset(linked_list *list, uint64_t offset);

/**
 * Delete a given entry from the given list.
 *
 * @param list Is the list to delete member from
 * @param entry Is the entry to delete
 */
void ll_del_entry(linked_list *list, ll_entry *entry);

/**
 * Create a new linked list.
 *
 * @return pointer to the new list on success or NULL on error.
 */
static inline linked_list *new_linked_list(void) {
    return (linked_list *)calloc(1, sizeof(linked_list));
}

/**
 * Delete a pre-existing linked list, freeing all the entries
 * as well as the associated data.
 */
void delete_linked_list(linked_list *list);

#endif // __UNET_LLIST_H__
