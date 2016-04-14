cmake_minimum_required(VERSION 3.3)
project(mb-5pf C)

include_directories(include)

set(INCLUDE_FILES
        include/common.h
        include/utils.h

        include/cpu_state.h
        include/memory.h
        include/register.h

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
        src/register.c

        src/if_stage.c
        src/id_stage.c
        src/ex_stage.c
        src/mem_stage.c
        src/wb_stage.c
)

add_executable(mb-5pf ${SOURCE_FILES} ${INCLUDE_FILES})
