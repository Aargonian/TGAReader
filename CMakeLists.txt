cmake_minimum_required(VERSION 3.0)
project(TGAReader)

set(SOURCE_FILES
        src/test.c
        src/image.c
        src/Util.c
        src/TGAImage.c
        src/Decode/TGADecode.c)

add_executable(TGAReader ${SOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR}/include)