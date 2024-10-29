CC = gcc
CFLAGS = -fPIC -Iinclude
LDFLAGS = -shared

SRC_DIR = src
INC_DIR = include
LIB_DIR = lib

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

LIB_NAME = arena
LIB_SO = $(LIB_DIR)/lib$(LIB_NAME).so

all: $(LIB_SO)

$(LIB_SO): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	mkdir -p $(LIB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB_SO)

install: $(LIB_SO)
	cp $(LIB_SO) /usr/lib/
	cp include/arena.h /usr/include/
	ldconfig

.PHONY: all clean install
