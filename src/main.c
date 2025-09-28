
#include <stdio.h>
#include <stdint.h>

#include "arp/arp.h"
#include "buffer/buffer.h"
#include "flexarr/flex.h"
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

    e = ipv4_get_route_to_host(0x1100);
    printf("Got %p\n", e);

    linked_list *arp_table = new_linked_list();
    if (arp_table == NULL) {
        fprintf(stderr, "No linked list for us :(\n");
        return 1;
    }

    flexible_array *arr = new_flex_array(sizeof(uint32_t), 1);
    farr_alloc(arr, 10);
    uint32_t *d = (uint32_t *)arr->data_array;
    for (uint32_t i = 0; i < 10; i++) {
        d[i] = i;
        printf("d[%d] at %p: %d\n", i, (void *)&d[i], d[i]);
    }
    farr_del_entry(arr, 3);
    for (uint32_t i = 0; i < 10; i++) {
        printf("d[%d] at %p: %d\n", i, (void *)&d[i], d[i]);
    }

    return 0;
}

