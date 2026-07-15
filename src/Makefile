CC = gcc
CFLAGS = -I./ -I../ -I../stdlib

SRC := $(shell find . -name '*.c')
MAIN_SRC := $(wildcard *.c)
LIB_SRC := $(filter-out $(MAIN_SRC), $(SRC))

MAIN_OBJ := $(patsubst %.c, ../build/%.o, $(notdir $(MAIN_SRC)))
LIB_OBJ  := $(patsubst %.c, ../build/%.o, $(notdir $(LIB_SRC)))

OBJ      := $(MAIN_OBJ) $(LIB_OBJ)
TARGET := $(patsubst %.c, ../dist/%.exe, $(MAIN_SRC))

main : $(TARGET)

o_o : CFLAGS += -O0 -march=native
o_o : main

o_one : CFLAGS += -O1 -march=native
o_one : main

o_two : CFLAGS += -O2 -march=native
o_two : main

o_three : CFLAGS += -O3 -march=native
o_three : main

byte : CFLAGS += -O2 -march=native
byte : post_build

debug : CFLAGS += -O0 -g
debug : post_build

all: o_three

../dist/%.exe: ../build/%.o $(LIB_OBJ) | ../dist
	$(CC) $(CFLAGS) $^ -o $@

../build/%.o: ../build
	$(CC) $(CFLAGS) -c $(shell find . -name $*.c) -o $@

../build :
	mkdir -p ../build

../dist :
	mkdir -p ../dist

post_build: main
	mv ../dist/bytescript.exe ../bytescript.exe
	rm -rf ../build
	rm -rf ../dist
	
dis : CFLAGS += -O2
dis : clean all
	objdump -d ../build/dised.o > ../dist/dised.txt