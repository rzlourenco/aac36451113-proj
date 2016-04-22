cmake_minimum_required(VERSION 3.3)
project(mb-t C)

include_directories(include)

set(INCLUDE_FILES
        include/common.h
        include/utils.h

        include/cpu_state.h
        include/memory.h
        include/register.h
)

set(SOURCE_FILES
        src/main.c
        src/utils.c

        src/cpu_state.c
        src/memory.c
        src/register.c
)

add_executable(mb-t ${SOURCE_FILES} ${INCLUDE_FILES})
