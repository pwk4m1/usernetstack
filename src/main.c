
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "arp/arp.h"
#include "buffer/buffer.h"
#include "iface/iface.h"
#include "link/eth/eth.h"
#include "llist/llist.h"
#include "route/route.h"

ipv4_routing_table *table = 0;

int main(void) {
    buffer *foo = new_buffer(32);
    net_interface *iface = create_interface("test-iface", create_eth_link, NULL);
    iface->state = up;

    table = new_ipv4_route_table();
    if (!table) {
        fprintf(stderr, "Can't create routing table!\n");
        return 1;
    }
    printf("Add route\n");
    add_ipv4_route_entry(false, 0xffff, 0xff, 0, iface);
    printf("Get route to host\n");
    ipv4_route_entry *e = ipv4_get_route_to_host(10);

    if (e) {
        printf("Found route: %p\n", e);
        iface_tx(e->iface, foo);
    }
    linked_list *table = new_linked_list();

    uint64_t cnt = arp_find_neighbours(table, iface);
    printf("Got %lld\n", cnt);
    if (!cnt) {
        fprintf(stderr, "%d / %s\n", errno, strerror(errno));
    }

    return 0;
}

