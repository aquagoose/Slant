cmake_minimum_required(VERSION 4.1)
project(Slant C)

set(CMAKE_C_STANDARD 99)

option(SLANT_BUILD_TESTS "Build the tests" ON)

file(GLOB SLANT_SRC CONFIGURE_DEPENDS
        include/Slant/*.h
        src/*.c
)

add_library(Slant SHARED ${SLANT_SRC})
target_include_directories(Slant PUBLIC include/)

if (SLANT_BUILD_TESTS)
    add_subdirectory(tests/)
endif ()