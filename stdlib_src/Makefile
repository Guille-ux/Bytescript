CC = gcc
CFLAGS = -I./ -I../ -I../src -I../dist

SRC := $(wildcard */*.c)
OBJS := $(SRC:.c=.o)
DLL := $(SRC:%.c=../dist/stdlib/%.dll)

OBJ := $(MATH_OBJ:%.o=../build/%.o)
C_API := ../dist/bytescript.a
TARGET := $(DLL)

.PRECIOUS: $(OBJ)

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
byte : main

debug : CFLAGS += -O0 -g
debug : main

all: o_three

../dist/stdlib/%.dll: ../build/%.o ../dist/stdlib/%_map.bs | ../dist/stdlib
	$(CC) $(CFLAGS) -shared ../build/$(notdir $<) $(C_API) -o ../dist/stdlib/$(notdir $@)

../dist/stdlib/%.bs: ../dist/stdlib
	cp $*.bs ../dist/stdlib

../build/%.o: %.c | ../build
	$(CC) $(CFLAGS) -c $< -o ../build/$(notdir $@)

../build:
	mkdir -p ../build

../dist/stdlib:
	mkdir -p ../dist/stdlib