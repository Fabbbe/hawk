BUILD_DIR ?= ./build
SRC_DIR ?= ./src
PROGRAM_NAME=hawk
IN_FILES=shader.o object.o 

CC=gcc
CFLAGS=-Wall -I./src -lm -lSDL2 -O2 -lGL -lGLEW -lcglm -g3# -DNDEBUG

$(PROGRAM_NAME): src/main.c $(IN_FILES)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(PROGRAM_NAME) $(IN_FILES)
