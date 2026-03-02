cmake_minimum_required(VERSION 3.20)
project(mixr)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(files include/mixr/mixr.hpp
        src/Context.cpp
        include/mixr/Context.h
        src/Impl/Impl.cpp
        src/Impl/Impl.h
        include/mixr/Common.h
        src/AudioBuffer.cpp
        include/mixr/AudioBuffer.h
        src/AudioSource.cpp
        include/mixr/AudioSource.h
        src/AudioDevice.cpp
        include/mixr/AudioDevice.h
        include/mixr/mixr.h
        src/C/Mixr.cpp
        include/mixr/Stream/AudioStream.hpp
        src/Stream/Wav.cpp
        include/mixr/Stream/Wav.hpp
        include/mixr/Stream/AudioStream.h
        src/C/Stream.cpp
        include/mixr/Stream/Wav.h
        src/C/Wav.cpp
        include/mixr/Utils/ADPCM.h
        src/Utils/ADPCM.cpp
        include/mixr/Stream/Flac.hpp
        src/Stream/Flac.cpp
        include/mixr/Stream/Flac.h
        src/C/Flac.cpp
        include/mixr/Stream/Vorbis.hpp
        src/Stream/Vorbis.cpp
        include/mixr/Stream/Vorbis.h
        src/C/Vorbis.cpp
        include/mixr/Stream/Mp3.hpp
        src/Stream/Mp3.cpp
        lib/minimp3/minimp3.h
        lib/minimp3/minimp3_ex.h
        include/mixr/Stream/Mp3.h
        src/C/Mp3.cpp
        src/Stream/StringUtils.h
        src/Stream/StringUtils.cpp
)

if (WIN32)
    list(APPEND files src/Device/SdlDevice.h src/Device/SdlDevice.cpp)
elseif (LINUX)
    list(APPEND files src/Device/AlsaDevice.h src/Device/AlsaDevice.cpp)
endif()

add_library(mixr SHARED ${files})

target_include_directories(mixr PUBLIC include)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (WIN32)
    add_subdirectory(lib/SDL)
    target_link_libraries(mixr PUBLIC SDL2::SDL2-static)
endif ()

add_subdirectory(lib/stb)
target_link_libraries(mixr PUBLIC stb)

if (LINUX)
    target_link_libraries(mixr PUBLIC asound)
endif()

set(WITH_OGG OFF CACHE BOOL "" FORCE)
set(INSTALL_MANPAGES OFF CACHE BOOL "" FORCE)
set(ENABLE_MULTITHREADING OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/flac)
target_link_libraries(mixr PUBLIC FLAC::FLAC++)

target_include_directories(mixr PUBLIC lib/minimp3)

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    target_compile_definitions(mixr PUBLIC _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
elseif (WIN32)
    target_link_options(mixr PRIVATE -static)
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")
endif ()

add_executable(mixrtest tests/main.cpp)
target_link_libraries(mixrtest PRIVATE mixr)