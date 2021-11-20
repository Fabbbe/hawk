BUILD_DIR ?= ./build
SRC_DIR ?= ./src
PROGRAM_NAME=hawk
IN_FILES=shader.o

CC=gcc
CFLAGS= -I./src -lm -lSDL2 -O2 -lGL -lGLEW -lglut# -DNDEBUG

$(PROGRAM_NAME): src/main.c $(IN_FILES)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(PROGRAM_NAME) $(IN_FILES)
