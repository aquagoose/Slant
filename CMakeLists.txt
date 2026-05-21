cmake_minimum_required(VERSION 3.20)
project(Slant)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB SLANT++_SRC CONFIGURE_DEPENDS
        include/Slant++/*.h
        include/Slant++/Utils/*.h
        include/Slant++/Stream/*.h
        src/Slant++/*.h
        src/Slant++/*.cpp
        src/Slant++/Stream/*.h
        src/Slant++/Stream/*.cpp
        src/Slant++/Utils/*.h
        src/Slant++/Utils/*.cpp
        
        src/Impl/*.h
        src/Impl/*.cpp
)

if (WIN32)
    list(APPEND SLANT++_SRC src/Slant++/Device/SdlDevice.h src/Slant++/Device/SdlDevice.cpp)
elseif (LINUX)
    list(APPEND SLANT++_SRC src/Slant++/Device/AlsaDevice.h src/Slant++/Device/AlsaDevice.cpp)
endif()

add_library(Slant++ STATIC ${SLANT++_SRC})
target_include_directories(Slant++ PUBLIC include/)
target_include_directories(Slant++ PRIVATE src/Impl)

if (WIN32)
    add_subdirectory(lib/SDL)
    target_link_libraries(Slant++ PUBLIC SDL2::SDL2-static)
endif ()

add_subdirectory(lib/stb)
target_link_libraries(Slant++ PUBLIC stb)

if (LINUX)
    target_link_libraries(Slant++ PUBLIC asound)
endif()

set(WITH_OGG OFF CACHE BOOL "" FORCE)
set(INSTALL_MANPAGES OFF CACHE BOOL "" FORCE)
set(ENABLE_MULTITHREADING OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/flac)
target_link_libraries(Slant++ PUBLIC FLAC::FLAC++)

target_include_directories(Slant++ PUBLIC lib/minimp3)

file(GLOB SLANT_SRC CONFIGURE_DEPENDS
        include/Slant/*.h
        include/Slant/Stream/*.h
        src/Slant/*.h
        src/Slant/*.cpp
)

add_library(slant SHARED ${SLANT_SRC})
target_include_directories(slant PUBLIC include/)
target_include_directories(slant PRIVATE src/Impl)
target_link_libraries(slant PUBLIC Slant++)

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    target_compile_definitions(Slant++ PUBLIC _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
elseif (WIN32)
    target_link_options(Slant++ PRIVATE -static)
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")
endif ()

add_executable(test_slant++ tests/test_slant++.cpp)
target_link_libraries(test_slant++ PRIVATE Slant++)

add_executable(test_slant tests/test_slant.c)
target_link_libraries(test_slant PRIVATE slant)