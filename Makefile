CC          = clang
WINCC       = x86_64-w64-mingw32-gcc

CFLAGS      = -Wall -Wextra -Werror -Wpedantic -std=c99
LFLAGS      = -lncurses
WINLFLAGS   = -lpdcurses

TARGET      = fears
WIN_TARGET  = fears.exe

SOURCES     = $(wildcard src/*.c)

all: clean build

build:
	$(CC) $(CFLAGS) $(SOURCES) $(LFLAGS) -o $(TARGET)

windows:
	$(WINCC) $(CFLAGS) $(SOURCES) $(WIN_LIBS) -o $(WIN_TARGET)

clean:
	rm -f $(TARGET) $(WIN_TARGET)

.PHONY: all build windows clean
