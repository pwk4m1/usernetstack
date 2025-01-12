cmake_minimum_required(VERSION 3.2)
project(netlib LANGUAGES C VERSION 0.5)

add_executable(netlib 
    src/main.c
    src/csum.c
    src/data_util.c
    src/udp.c
    src/ip.c
    src/eth.c
    src/socket.c
    src/link.c
    src/slip.c
)

if (CMAKE_SYSTEM_NAME STREQUAL "LF-OS")
    target_sources(netlib PRIVATE
        src/platform/lf_os/socket.c
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(netlib PRIVATE
        src/platform/linux/socket.c
    )
else()
    error("Unsupported platform")
endif()

target_include_directories(netlib SYSTEM PUBLIC
    "src/include"
)

target_compile_options(netlib PRIVATE
    -Wall -Wextra -Wpedantic -O2 
)

set_target_properties(netlib PROPERTIES OUTPUT_NAME netlib)

