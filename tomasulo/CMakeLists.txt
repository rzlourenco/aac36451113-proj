cmake_minimum_required(VERSION 3.3)
project(mb-t C)

include_directories(include)

set(INCLUDE_FILES
        include/common.h
        include/utils.h

        include/cpu.h
        include/memory.h

        include/branch_predictor.h
        include/cdb.h
        include/commit.h
        include/execute.h
        include/fetch.h
        include/issue.h
        include/register.h
        include/rob.h
)

set(SOURCE_FILES
        src/main.c
        src/utils.c

        src/cpu.c
        src/memory.c

        src/branch_predictor.c
        src/cdb.c
        src/commit.c
        src/execute.c
        src/fetch.c
        src/issue.c
        src/register.c
        src/rob.c
)

add_executable(mb-t ${SOURCE_FILES} ${INCLUDE_FILES})
