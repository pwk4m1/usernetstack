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
#ifndef __UNET_IP_H__
#define __UNET_IP_H__

#include <sys/types.h>
#include <stdint.h>

#include "../buffer/buffer.h"

/* IPv4 option class definitions */
enum IPV4_OPTION_CLASS {
    CONTROL,
    DEBUG = 2
};

/**
 * IPv4 option number definitions:
 * 
 * @member END_OF_OPTION_LIST    -- This option occupies only 1 octet; it has no length octet.
 * @member NO_OPERATION          -- This option occupies only 1 octet; it has no length octet
 * @member SECURITY              -- Used to carry Security, Compartmentation, User Group (TCC), and 
 *                                  Handling Restriction Codes compatible with DOD requirements.
 * @member LOOSE_SOURCE_ROUTING  -- Used to route the internet datagram based on information supplied by the source.
 * @member INTERNET_TIMESTAMP    -- Internet timestamp :b
 * @member RECORD_ROUTE          -- Used to trace the route an internet datagram takes.
 * @member STREAM_ID             -- Used to carry the stream identifier.
 * @member STRICT_SOURCE_ROUTING -- Used to route the itnernet datagram based on information supplied by the source.
 *
 */ 
enum IPV4_OPTION_NUMBER {
    END_OF_OPTION_LIST,
    NO_OPERATION,
    SECURITY,
    LOOSE_SOURCE_ROUTING,
    INTERNET_TIMESTAMP,
    RECORD_ROUTE = 7,
    STREAM_ID = 8,
    STRICT_SOURCE_ROUTING = 9
};

/** 
 * Option type structure
 *
 * @member copied_flags bit is set to indicate that this option is copied into all fragments of fragmentation
 * @member opt_class bits set control or debug/measurement option
 * @member opt_num Please refer to IPV4_OPTION_NUMBER documentation
 */
typedef struct __attribute__((packed)) {
    int copied_flags                 : 1;
    enum IPV4_OPTION_CLASS opt_class : 2;
    enum IPV4_OPTION_NUMBER opt_num  : 5;
    uint8_t option_len;
    uint8_t *option_octets;
} ipv4_option_structure;

/** 
 * IPv4 Security option security field
 *
 */
enum IPV4_SECURITY_SFIELD {
    UNCLASSIFIED = 0x0000,
    CONFIDENTIAL = 0xF135,
    EFTO = 0x789A,
    MMMM = 0xBC4D,
    PROG = 0x5E26,
    RESTRICTED = 0xAF13,
    SECRET = 0xD788,
    TOP_SECRET = 0x6BC5
};

/** 
 * Security option structure
 *
 * @member sfield is the security classification
 * @member compartments is awful, I don't even want to know, refer to DoD 
 * @member handling_restrictions -- refer to DoD
 * @member transmission_control_high -- refer to DoD
 * @member transmission_control_low -- refer to DoD
 */
typedef struct {
    enum IPV4_SECURITY_SFIELD sfield;
    uint16_t compartments;
    uint16_t handling_restrictions;
    uint16_t transmission_control_high;
    uint8_t transmission_control_low;
} ipv4_security_option_fields;

/** 
 * Possible Type of Service (TOS) precedence values to use
 *
 * "The Network Control precedence designation is intended to be used
 * within a network only.The actual use and control of that
 * designation is up to each network. The Internetwork Control
 * designation is intended for use by gateway control originators only.
 * If the actual use of these precedence designations is of concern to
 * a particular network, it is the responsibility of that network to
 * control the access to, and use of, those precedence designations."
 * -- rfc 791 (https://datatracker.ietf.org/doc/html/rfc791)
 */
enum IPV4_TOS_PRE {
    ROUTINE,
    PRIORITY,
    IMMEDIATE,
    FLASH,
    FLASH_OVERRIDE,
    ECP,
    INTERNETWORK_CTRL,
    NETWORK_CTRL
};  

/**
 * ipv4 flags structure
 *
 * @member reserved Must be set to 0
 * @member dont_fragment Is set to 1 if packet may be fragmented
 * @member last_fragment Is set to 1 if this is last fragment
 */
enum IPV4_FLAGS {
    DONT_FRAGMENT = (1 << 6),
    LAST_FRAGMENT = (1 << 7)
};

/** 
 * IPv4 Header structure ( https://datatracker.ietf.org/doc/html/rfc791#section-3.1 )
 *
 * @member version Is a 4 bit version field
 * @member ihl Is the length of the header
 * @member tos Is the type of servce
 * @member len Is the total length
 * @member id Is the identification
 * @member flags For this, refer to enum IPV4_FLAGS 
 * @membef foff Is the fragment offset
 * @member ttl For time to live
 * @member ptcl Is the protocol identifier for next protocol header (icmp, tcp, udp, ..)
 * @member csum Is the IPv4 checksum
 * @member src Marks the source ipv4 address
 * @member dst Marks the destination ipv4 address
 */
typedef struct __attribute__((packed)) {
    unsigned ihl          : 4;
    unsigned version      : 4;
    enum IPV4_TOS_PRE tos : 8;
    unsigned len          : 16;
    unsigned id           : 16;
    unsigned flags_foff   : 16;
    unsigned ttl          : 8;
    unsigned ptcl         : 8;
    unsigned csum         : 16;
    unsigned src          : 32;
    unsigned dst          : 32;
} ipv4_header;

/**
 * Send a packet over a IPv4 to a given destination address.
 *
 * @param host is the destination address
 * @parma ptcl is the protocol identifier to use
 * @param data is a pointer to buffer of data to transmit
 * @param src is optional source address. Set to NULL for default
 * @param options is a pointer to a populated ipv4_option_structure or NULL for none 
 * @return uint16_t amount of bytes sent or 0 on error. Sets errno on error.
 */
uint16_t ipv4_transmit(uint32_t host, uint8_t ptcl, buffer *data, uint32_t src, ipv4_option_structure *options);

#endif // __UNET_IP_H__
