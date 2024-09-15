cmake_minimum_required(VERSION 3.2)
project(netlib LANGUAGES C VERSION 0.5)

add_executable(netlib 
    src/main.c
    src/csum.c
    src/data_util.c
    src/udp.c
    src/ip.c
)

target_compile_options(netlib PRIVATE
    -Wall -Wextra -Wpedantic -O2
)

set_target_properties(netlib PROPERTIES OUTPUT_NAME netlib)

