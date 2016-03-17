cmake_minimum_required(VERSION 3.3)
project(aac_proj)

option(USE_CLANG "Use Clang to compile" ON)

if (USE_CLANG)
    set(CMAKE_C_COMPILER   "clang")
    set(CMAKE_CXX_COMPILER "clang++")
endif(USE_CLANG)

set(COMMON_C_FLAGS "-Wall -Wextra --std=c11 --pedantic-errors")

set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}       ${COMMON_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMMON_C_FLAGS} -DDEBUG=1 -ggdb -fsanitize=address,integer,undefined")

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
