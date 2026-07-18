CC = gcc
CFLAGS = -I./ -I../ -I../stdlib_src

SRC := $(shell find . -name '*.c')
MAIN_SRC := $(wildcard *.c)
LIB_SRC := $(filter-out $(MAIN_SRC), $(SRC))
MAPS_SRC := $(wildcard Globals/maps/*.bs)

MAIN_OBJ := $(patsubst %.c, ../build/%.o, $(notdir $(MAIN_SRC)))
LIB_OBJ  := $(patsubst %.c, ../build/%.o, $(notdir $(LIB_SRC)))

OBJ      := $(MAIN_OBJ) $(LIB_OBJ)
MAPS   := $(MAPS_SRC:%.bs=../dist/stdlib/%.bs)
TARGET := $(patsubst %.c, ../dist/%.exe, $(MAIN_SRC))

main : $(TARGET) $(MAPS)

o_o : CFLAGS += -O0 -march=native
o_o : main

o_one : CFLAGS += -O1 -march=native
o_one : main

o_two : CFLAGS += -O2 -march=native
o_two : main

o_three : CFLAGS += -O3 -march=native
o_three : main

byte : CFLAGS += -O2 -march=native
byte : main

debug : CFLAGS += -O0 -g
debug : main

all: o_three

../dist/%.exe: ../build/%.o $(LIB_OBJ) | ../dist
	$(CC) $(CFLAGS) $^ -o $@

../build/%.o: ../build
	$(CC) $(CFLAGS) -c $(shell find . -name $*.c) -o $@

../dist/stdlib/%.bs: ../dist/stdlib
	cp $*.bs ../dist/stdlib

../build :
	mkdir -p ../build

../dist :
	mkdir -p ../dist

../dist/stdlib:
	mkdir -p ../dist/stdlib

dis : CFLAGS += -O2
dis : clean all
	objdump -d ../build/dised.o > ../dist/dised.txt