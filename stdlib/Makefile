CC = gcc
CFLAGS = -I./ -I../ -I../src

MATH_SRC := $(wildcard Math/*.c)
MATH_OBJ := $(MATH_SRC:.c=.o)
MATH := $(MATH_SRC:%.c=../dist/%.dll)

OBJ := $(MATH_OBJ:%.o=../build/%.o)
TARGET := $(MATH)

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

../dist/%.dll: ../build/%.o | ../dist
	$(CC) $(CFLAGS) -shared ../build/$(notdir $<) -o ../dist/$(notdir $@)

../build/%.o: %.c | ../build
	$(CC) $(CFLAGS) -c $< -o ../build/$(notdir $@)

../build:
	mkdir -p ../build

../dist:
	mkdir -p ../dist