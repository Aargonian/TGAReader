cmake_minimum_required(VERSION 3.1)
project(TGAReader C)

set(GNUCC_WARNINGS "-Wall -pedantic -Wextra -Wformat -Wcast-align              \
        -Wpointer-arith -Wbad-function-cast -Wmissing-prototypes               \
        -Wstrict-prototypes -Wmissing-declarations -Winline -Wundef            \
        -Wnested-externs -Wcast-qual -Wshadow -Wwrite-strings                  \
        -Wunused-parameter -Wfloat-equal")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_TGA_DEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DDEBUG=0")


set(SOURCE_FILES
		include/TGAImage.h
        src/test.c
        src/TGAImage.c
        src/TGADecode.c
        src/TGAEncode.c
		src/Private/TGAPrivate.h
        )

IF (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GNUCC_WARNINGS} -pg -O0")
ENDIF (CMAKE_COMPILER_IS_GNUCC)

add_executable(TGAReader ${SOURCE_FILES})

target_include_directories(TGAReader PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(TGAReader PUBLIC m)
