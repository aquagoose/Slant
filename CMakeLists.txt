cmake_minimum_required(VERSION 3.25)
project(mixr C)

set(CMAKE_C_STANDARD 11)

file(GLOB MIXR_SRC CONFIGURE_DEPENDS
        include/mixr/*.h
        src/mixr/*.h
        src/mixr/*.c
        src/mixr/devices/*.h
        src/mixr/devices/*.c
)

add_library(mixr SHARED ${MIXR_SRC})
target_include_directories(mixr PUBLIC include/)

add_subdirectory(external/SDL)
target_link_libraries(mixr PRIVATE SDL3::SDL3)

function(mixr_tests dir name)
    message(STATUS "Searching for tests in ${dir}")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")
    file(GLOB TEST_SRC ${dir}/*.c)
    foreach (file IN LISTS TEST_SRC)
        get_filename_component(filename ${file} NAME_WE)
        set(testname "Test_${name}_${filename}")
        message(STATUS "${testname} -> ${file}")
        add_executable(${testname} ${file})
        target_link_libraries(${testname} PRIVATE mixr)
    endforeach ()
    list(POP_BACK CMAKE_MESSAGE_INDENT)
endfunction()

mixr_tests(tests/mixr "Mixr")