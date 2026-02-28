#ifndef WINDOW_H
#define WINDOW_H

#include "buffer.h"

typedef struct {
    buffer* buf;
    int winrows;
    int wincols;
    int cy;
    int cx;
    int rowoff;
    int coloff;
} window;

typedef struct {
    window* windows;
    int numwins;
    int scrrows;
    int scrcols;
} screen;

void drawScreen(screen*);
void drawWin(window*);

#endif
