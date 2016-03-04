cmake_minimum_required(VERSION 3.3)
project(aac_proj)

set(INCLUDE_FILES
        )

set(SOURCE_FILES
        main.c)

add_executable(microblaze ${SOURCE_FILES} ${INCLUDE_FILES})
