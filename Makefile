# Makefile
BUILD_DIR ?= ./build
SRC_DIR ?= ./src
PROGRAM_NAME=hawk
IN_FILES=shader.o object.o scene.o player.o

CC=gcc
PKG_CONFIG=$(shell pkg-config --cflags --libs libxml-2.0 sdl2 gl glew cglm)
CFLAGS=-Wall -I./src -lm -O2 -g3 $(PKG_CONFIG) #-DNDEBUG

$(PROGRAM_NAME): src/main.c $(IN_FILES)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(PROGRAM_NAME) $(IN_FILES)
	rm -rf *.log
