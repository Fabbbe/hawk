BUILD_DIR ?= ./build
SRC_DIR ?= ./src
PROGRAM_NAME=hawk
IN_FILES=

CC=gcc
CFLAGS=-lm -lSDL2 -O2 -lGL -lGLEW -lglut# -DNDEBUG

$(PROGRAM_NAME): src/main.c $(IN_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

