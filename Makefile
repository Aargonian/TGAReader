CC = gcc
CXX = g++
LINTER = cppcheck

WARNINGS = -Wall -pedantic \
		   -Wextra \
		   -Wdouble-promotion \
		   -Wformat \
		   -Wcast-align \
		   -Wpointer-arith \
		   -Wbad-function-cast \
		   -Wmissing-prototypes \
		   -Wstrict-prototypes \
		   -Wmissing-declarations \
		   -Winline \
		   -Wundef \
		   -Wnested-externs \
		   -Wcast-qual \
		   -Wshadow \
		   -Wwrite-strings \
		   -Wunused-parameter \
		   -Wfloat-equal

LINTER_OPTIONS = --enable=style --inconclusive

FLAGS = -g $(WARNINGS)
C11_FLAGS = $(FLAGS) -std=c11 -DC11
C99_FLAGS = $(FLAGS) -std=c99 -DC99
CFLAGS = $(C99_FLAGS)
CXXFLAGS = $(FLAGS) -std=c++11
PROG_NAME = image_tester
INCLUDE_DIR = include/
DEFINES = -DDEBUG
LIBS = gtk+-3.0

# List of all files in program. Should be updated explicitly.
FILES = src/test.c \
	src/image.c \
	src/TGAImage.c \
	src/Decode/TGADecode.c \
        src/Util.c

build:
	$(CC) $(CFLAGS) $(FILES) -I $(INCLUDE_DIR) $(DEFINES) -o bin/$(PROG_NAME) `pkg-config --cflags $(LIBS) --libs $(LIBS)`

lint:
	$(LINTER) $(LINTER_OPTIONS) -I $(INCLUDE_DIR) $(FILES)
