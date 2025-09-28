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
#ifndef __UNET_FLEX_H__
#define __UNET_FLEX_H__

#include <stdbool.h>
#include <stdint.h>

/**
 * Helper structure for flexible sized arrays
 *
 * @member data_array is a pointer to the actual array
 * @member memb_size defines size of a single entry in the array
 * @member arr_size keeps track of size of the array
 */
typedef struct {
    void *data_array;
    uint32_t memb_size;
    uint32_t arr_size;
} flexible_array;

/**
 * Create a new flexible array
 *
 * @param memb_size Defines size of a single entry in the array
 * @param size Gives the size of array to allocate
 * @return a pointer to new array on success or NULL on error
 */
flexible_array *new_flex_array(uint32_t memb_size, uint32_t arr_size);

/**
 * Allocate space for a new entry to the flexible array
 *
 * @param array Is the array to add this member to
 * @param count Is the amount of new members to allocate space for 
 * @return bool success
 */
bool farr_alloc(flexible_array *array, uint32_t count); 

/**
 * Get address to Nth member in the array
 *
 * @param array Is the array to get the member from
 * @param which Is the N
 * @return pointer to the Nth entry on success or -1 on error
 */
void *farr_get_entry(flexible_array *array, uint32_t which);

/**
 * Remove Nth entry in the array
 *
 * @param array Is the array to remove a member from
 * @param which is the N
 */
void farr_del_entry(flexible_array *array, uint32_t which);

#endif
