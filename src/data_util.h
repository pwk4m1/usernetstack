/* Helper structures, definitions, and function declarations that are related
 * to abstracting away varying data types, so that each protocol layer as is
 * independent of eachother as possible, and that read/write, etc. are as 
 * uniform between different protocols.
 *
 */
#ifndef __NETLIB_DATA_UTIL_H__
#define __NETLIB_DATA_UTIL_H__

#include <sys/types.h>
#include <sys/socket.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

/* Various state definitions a connection can be in. 
 *
 * @param idle -- There's nothing happening regarding this connection right now
 * @param listening -- Waiting for 3-way handshake from remote peer
 * @param connecting -- Performing a 3-way handshake with remote peer
 * @param connected -- Connected over TCP to a remote peer 
 * @param disconnected -- TCP stream was created, but it's been disconnected
 * @param waiting -- Waiting for remote peer to send us datagrams
 * @param reading -- Reading a datagram from remote peer
 * @param writing -- Sending a datagram to remote peer
 */
enum CONNECTION_STATUS {
    IDLE,
    LISTENING,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    WAITING,
    READING,
    WRITING
};

/* connection_data structure holds information about a specific
 * connection, regardless of if it's a stream connection (TCP) that lives
 * long, or just simple ICMP or UDP or any other form of datagram that's
 * sent once and then forgotten about.
 *
 * We need to keep track of source/destination IP addresses, IP version,
 * connection type (priority, etc.), used source port, IP ID, status
 * of the connection (mostly relevant for TCP), etc.
 *
 * @param int sockfd                    -- Raw socket associated with this
 *                                         this connection
 * @param struct socakddr *source       -- keep track of source IP, family 
 * @param struct sockaddr *destination  -- .. destination IP, family 
 * @param uint16_t source_port          -- TCP or UDP source port
 * @param uint16_t ip_id                -- ID of this connection
 * @param enum CONNECTION_STATUS state  -- state of this connection
 */
typedef struct {
    struct sockaddr *source;
    struct sockaddr *destination;
    uint16_t source_port;
    uint16_t ip_id;
    enum CONNECTION_STATUS state;
} connection_data;

/* Helper to populate sockaddr_in and sockaddr_in6 structures from
 * user provided data.
 *
 * @param int family        -- AF_INET or AF_INET6
 * @param char *addr        -- Pointer to ip address as ascii string
 * @param uint16_t port     -- tcp/udp port we'll use
 * @return pointer to populated struct sockaddr
 */
struct sockaddr *populate_sockaddr(int family, char *addr, uint16_t port);

/* Helper to create continuous packet from various protocol header structures
 * and user-provided payload.
 *
 * @param void **pkt    -- Pointer to use for crafting the packet
 * @param void *iphdr   -- Pointer to IP or pseudo IP header
 * @param size_t iplen  -- Size of provided IP or pseudo header
 * @param void *ptclhdr -- UDP or TCP header
 * @param size_t ptclen -- Size of provided UDP or TCP header
 * @param void *payload -- Data to transmit
 * @param size_t len    -- Size of payload to transmit
 * @return size_t size of built packet
 *
 * NOTE: *pkt is allocated by this function for the caller
 */
size_t build_packet(void **pkt, void *iphdr, size_t iplen,
        void *ptclhdr, size_t ptclen,
        void *payload, size_t len);

#endif // __NETLIB_DATA_UTIL_H__
