cmake_minimum_required(VERSION 3.2)
project(unetlib LANGUAGES C VERSION 0.5)
set(PLATFORM "linux")

add_executable(unetlib
    src/main.c
    src/util.c
    src/iface/iface.c
    src/route/route.c
    src/llist/llist.c
    src/arp/arp.c
    src/flexarr/flex.c
)

add_subdirectory(src/link)

target_compile_options(unetlib PRIVATE
    -Wall -Wextra -Wpedantic -O2 
)

set_target_properties(unetlib PROPERTIES OUTPUT_NAME unetlib)

