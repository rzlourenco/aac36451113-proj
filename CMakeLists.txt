cmake_minimum_required(VERSION 3.3)
project(aac_proj C)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(COMMON_C_FLAGS
        "-Wall \
        -Wextra \
        -Wpedantic \
        --std=c11 \
        -D_POSIX_SOURCE \
        -Werror=pedantic \
        --pedantic-errors \
        -fstack-protector-strong \
        -fdiagnostics-color=always")

set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} ${COMMON_C_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} ${COMMON_C_FLAGS} \
        -UNDEBUG \
        -DDEBUG \
        -ggdb \
        -fsanitize=address,undefined")

set(CMAKE_C_FLAGS_RELEASE
        "${CMAKE_C_FLAGS_RELEASE} ${COMMON_C_FLAGS} \
        -D_FORTIFY_SOURCE=1 \
        -UDEBUG \
        -DNDEBUG \
        -Ofast")

set(CMAKE_C_FLAGS_RELWITHDEBINFO
        "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${COMMON_C_FLAGS} \
        -D_FORTIFY_SOURCE=1 \
        -ggdb \
        -Ofast")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(5-stage-multicycle)
add_subdirectory(5-stage-pipeline)
add_subdirectory(5-stage-pipeline-forwarding)
add_subdirectory(5-stage-pipeline-forwarding-bp)
add_subdirectory(tomasulo)
