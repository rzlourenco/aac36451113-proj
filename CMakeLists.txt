cmake_minimum_required(VERSION 3.3)
project(aac_proj)

option(USE_CLANG "Use Clang to compile" ON)

if (USE_CLANG)
    set(CMAKE_C_COMPILER   "clang")
    set(CMAKE_CXX_COMPILER "clang++")
endif(USE_CLANG)

set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}       -Wall -Wextra --std=c11 --pedantic-errors")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -fsanitize=address,integer,undefined")

set(INCLUDE_FILES
        common.h
        utils.h

        cpu_state.h
        memory.h
        reg_file.h

        if_stage.h
        id_stage.h
        ex_stage.h
        mem_stage.h
        wb_stage.h
        )

set(SOURCE_FILES
        main.c
        utils.c

        cpu_state.c
        memory.c
        reg_file.c

        if_stage.c
        id_stage.c
        ex_stage.c
        mem_stage.c
        wb_stage.c
        )

add_executable(microblaze ${SOURCE_FILES} ${INCLUDE_FILES})
