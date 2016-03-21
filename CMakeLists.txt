cmake_minimum_required(VERSION 3.3)
project(aac_proj)

set(COMMON_C_FLAGS "-Wall -Wextra -Wpedantic -Wuninitialized --std=c11 -D_POSIX_SOURCE --pedantic-errors")

set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}       ${COMMON_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMMON_C_FLAGS} -DDEBUG=1 -ggdb -fsanitize=address,undefined")

include_directories(include)

set(INCLUDE_FILES
        include/common.h
        include/utils.h

        include/cpu_state.h
        include/memory.h
        include/reg_file.h
        include/if_stage.h
        include/id_stage.h
        include/ex_stage.h
        include/mem_stage.h
        include/wb_stage.h
)

set(SOURCE_FILES
        src/main.c
        src/utils.c

        src/cpu_state.c
        src/memory.c
        src/reg_file.c
        src/if_stage.c
        src/id_stage.c
        src/ex_stage.c
        src/mem_stage.c
        src/wb_stage.c
)

add_executable(microblaze ${SOURCE_FILES} ${INCLUDE_FILES})
