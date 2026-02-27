CC = gcc
CFLAGS = -I./src
SRC := $(shell find src -name '*.c')
MAIN_SRC := $(wildcard src/*.c)
LIB_SRC := $(filter-out $(MAIN_SRC), $(SRC))
MAIN_OBJ := $(patsubst %.c, build/%.o, $(notdir $(MAIN_SRC)))
LIB_OBJ  := $(patsubst %.c, build/%.o, $(notdir $(LIB_SRC)))
OBJ      := $(MAIN_OBJ) $(LIB_OBJ)
TARGET := $(patsubst src/%.c, dist/%.exe, $(MAIN_SRC))


base : $(TARGET)

o_one : CFLAGS += -O1
o_one : base

o_two : CFLAGS += -O2
o_two : base

byte : CFLAGS += -O2
byte : post_build

all: CFLAGS += -O3
all: base



dist/%.exe: build/%.o $(LIB_OBJ) | dist
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: build
	$(CC) $(CFLAGS) -c $(shell find src -name $*.c) -o $@

build :
	mkdir -p build

dist :
	mkdir -p dist

clean : 
	rm -rf build
	rm -rf dist
	clear

clean_trash : clean
	rm bytescript.exe

post_build : clean base
	mv dist/bytescript.exe ./bytescript.exe
	rm -rf build
	rm -rf dist