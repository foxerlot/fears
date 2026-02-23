CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c99
LFLAGS = -lncurses

SOURCES = $(wildcard src/*.c)

all: clean build

build:
	$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o fears

clean:
	rm -f fears

.PHONY: all build clean
