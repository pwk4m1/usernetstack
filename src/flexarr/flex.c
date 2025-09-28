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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "flex.h"

/**
 * Create a new flexible array
 *
 * @param memb_size Defines size of a single entry in the array
 * @param size Gives the size of array to allocate
 * @return a pointer to new array on success or NULL on error
 */
flexible_array *new_flex_array(uint32_t memb_size, uint32_t arr_size) {
    flexible_array *ret = (flexible_array *)calloc(1, sizeof(flexible_array));
    if (ret) {
        ret->arr_size = arr_size;
        ret->memb_size = memb_size;
        ret->data_array = calloc(arr_size, memb_size);
        if (!ret->data_array) {
            free(ret);
            return NULL;
        }
    }
    return ret;
}

/**
 * Allocate space for a new entry to the flexible array
 *
 * @param array Is the array to add this member to
 * @param count Is the amount of new members to allocate space for 
 * @return bool success
 */
bool farr_alloc(flexible_array *array, uint32_t count) {
    uint32_t new_size = (count * array->memb_size) + (array->arr_size * array->memb_size);
    void *new = realloc(array->data_array, new_size);
    if (!new) {
        return false;
    }
    if (new != array->data_array) {
        array->data_array = new;
    }
    array->arr_size += count;
    return true;
}

/**
 * Get address to Nth member in the array
 *
 * @param array Is the array to get the member from
 * @param which Is the N
 * @return pointer to the Nth entry on success or -1 on error
 */
void *farr_get_entry(flexible_array *array, uint32_t which) {
    if (which > array->arr_size) {
        return (void *)-1; 
    }
    uint64_t addr = (uint64_t)array->data_array;
    addr += (which * array->memb_size);
    return (void *)addr;
}

/**
 * Remove Nth entry in the array
 *
 * @param array Is the array to remove a member from
 * @param which is the N
 */
void farr_del_entry(flexible_array *array, uint32_t which) {
    void *entry = farr_get_entry(array, which);
    if (entry == (void *)-1) {
        return;
    }
    void *remaining = farr_get_entry(array, which + 1);
    array->arr_size--;
    memcpy(entry, remaining, ((array->arr_size) * array->memb_size));
    array->data_array = realloc(array->data_array, (array->arr_size * array->memb_size));
}

