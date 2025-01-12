/* Helpers for creating IPv4 and IPv6 headers */
#ifndef __NETLIB_IP_H__
#define __NETLIB_IP_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <stdint.h>

#include "socket.h"

/* Initialise ip header system */
void ip_initialise(void);

/* Finalise ip header system */
void ip_finalise(void);

/* IPv4 option class definitions */
enum IPV4_OPTION_CLASS {
    CONTROL,
    DEBUG = 2
};

/* IPv4 option number definitions:
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

/* Option type structure
 *
 * @member int copied_flags                 -- Indicates that this option is copied into all fragments of fragmentation
 * @member enum IPV4_OPTION_CLASS opt_class -- Is this control or debug/measurement option
 * @member enum IPV4_OPTION_NUMBER opt_num  -- refer to IPV4_OPTION_NUMBER documentation
 */
typedef struct __attribute__((packed)) {
    int copied_flags                 : 1;
    enum IPV4_OPTION_CLASS opt_class : 2;
    enum IPV4_OPTION_NUMBER opt_num  : 5;
    uint8_t option_len;
    uint8_t *option_octets;
} ipv4_option_structure;

/* IPv4 Security option security field
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

/* Security option structure
 *
 * @member enum IPV4_SECURITY_SFIELD -- security classification
 * @member uint16_t compartments -- refer to DoD
 * @member uint16_t handling_restrictions -- refer to DoD
 * @member uint16_t transmission_control_high -- refer to DoD
 * @member uint8_t transmission_control_low -- refer to DoD
 */
typedef struct {
    enum IPV4_SECURITY_SFIELD sfield;
    uint16_t compartments;
    uint16_t handling_restrictions;
    uint16_t transmission_control_high;
    uint8_t transmission_control_low;
} ipv4_security_option_fields ;

/* Possible Type of Service (TOS) precedence values to use
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

/* ipv4 flags structure
 *
 * @member unsigned reserved      -- Always 0
 * @member unsigned dont_fragment -- set to 1 if packet may be fragmented
 * @member unsigned last_fragment -- set to 1 if this is last fragment
 */
enum IPV4_FLAGS {
    DONT_FRAGMENT = (1 << 6),
    LAST_FRAGMENT = (1 << 7)
};

/* IPv4 specific socket options that can be set for selecting various options
 * per connection in use. Not to be confused with internet options.
 *
 * @member enum IPV4_TOS_PRE pre                 -- Type of service precedence value
 * @member int low_delay                         -- Normal or low delay
 * @member int high_throughput                   -- Mark this as high throughput datagram
 * @member int high_reliability                  -- Mark this datagram requiring high reliability
 * @member uint8_t ttl                           -- Time to live value to use
 * @member struct ipv4_option_structure *options -- Pointer to populated IPv4 options structure if options are used,
 *                                                  or NULL if not
 * @member uint16_t mtu                          -- Maximum transmission unit
 */
typedef struct __attribute__((packed)) {
    unsigned int pre                 : 3;
    unsigned int low_delay           : 1;
    unsigned int high_throughput     : 1;
    unsigned int high_reliability    : 1;
    unsigned int no_fragment         : 1;
    uint8_t ttl;
    ipv4_option_structure *options;
    uint16_t mtu;
} ipv4_socket_options;

/* IPv4 Header structure ( https://datatracker.ietf.org/doc/html/rfc791#section-3.1 )
 *
 * @member unsigned version      -- 4 bit version field
 * @member unsigned ihl          -- internet header length
 * @member enum IPV4_TOS_PRE tos -- type of servce
 * @member unsigned len          -- total length
 * @member unsigned id           -- identification
 * @member unsigned flags        -- refer to enum IPV4_FLAGS 
 * @membef foff                  -- fragment offset
 * @member ttl                   -- time to live
 * @member ptcl                  -- protocol identifier for next protocol header (icmp, tcp, udp, ..)
 * @member csum                  -- checksum
 * @member src                   -- source ipv4 address
 * @member dst                   -- destination ipv4 address
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
} ipv4_hdr;

/* Allocate IPv4 header when non-standard header is required.
 *
 * @param struct sockaddr_in *src -- Pointer to source sockaddr_in struct
 * @param struct sockaddr_in *dst -- Pointer to destination sockaddr_in struct
 * @param uint8_t tos             -- Type of service value
 * @param uint16_t f_off_vcf      -- Fragment offset and control bits
 * @param uint8_t ttl             -- Time to live
 * @param uint8_t proto           -- Protocol
 * @param uint8_t option_type     -- Type field for additional IPv4 options or 0 if unused
 * @param uint8_t option_len      -- Length of additional option octets or 0 if unused 
 * @param uint8_t *option_buf     -- Pointer to remaining `option_len` options or 0 if unused
 * @param uint16_t tlen           -- Amount of bytes in next protocol header
 *                                   and payload we're delivering
 * @return pointer to populated ipv4 header structure on success or 0 on error
 */
ipv4_hdr *create_ipv4_hdr(struct sockaddr_in *src,
        struct sockaddr_in *dst, uint8_t tos, uint16_t f_off_vcf,
        uint8_t ttl, uint8_t proto, uint8_t option_type, uint8_t option_len,
        uint8_t *option_buf, uint16_t tlen);


/* Allocate and populate a default non-priority IPv4 header for user.
 * Unless there's a need for high priority delay, reliability or throughput, this
 * is likely the type of routine packet header you want.
 *
 * @param struct sockaddr_in *src -- Pointer to source sockaddr_in struct
 * @param struct sockaddr_in *dst -- Pointer to destination sockaddr_in struct
 * @param uint8_t proto           -- Protocol identification number
 * @param uint16_t tlen           -- Amount of bytes in next protocol header
 *                                   and payload we're delivering
 * @return pointer to populated ipv4 header structure
 */
inline ipv4_hdr *create_std_ipv4_hdr(struct sockaddr_in *src, 
        struct sockaddr_in *dst, uint8_t proto, uint16_t tlen) {
    return create_ipv4_hdr(src, dst, 16, 0, 64, proto, 0, 0, 0, tlen);
}

/* Transmit datagram over IPv4 protocol
 *
 * @param net_socket *socket        -- Pointer to populated net_socket structure
 * @param struct sockaddr_in *src   -- Pointer to populated source sockaddr_in structure
 * @param struct sockaddr_in *dst   -- Pointer to populated destination sockaddr_in structure
 * @param const void *data          -- Pointer to datagram to send, including appropriate
 *                                     protocol header
 * @param size_t data_len           -- Length of datagram to send
 * @return size_t amount of bytes sent excluding ip header on success or -1 on error.
 * Set errno on error.
 */
size_t ipv4_transmit_datagram(net_socket *socket, struct sockaddr_in *src,
        struct sockaddr_in *dst, const void *data, size_t data_len);

/* Receive datagram over IPv4 protocol
 *
 * @param net_socket *socket        -- Pointer to populated net_socket structure
 * @param const void *dst           -- Pointer to memory where we'll write received data to
 * @param size_t r_len              -- How many bytes do we want to receive *including* IPv4 header
 * @return size_t amount of bytes received on success or -1 on error.
 *                Set errno on error
 */
size_t ipv4_receive_datagram(net_socket *socket, void *dst, size_t r_len);

#endif // __NETLIB_IP_H__
