#ifndef WINDOW_H
#define WINDOW_H

#include "buffer.h"

typedef struct {
    int winrows;
    int wincols;
    int cy;
    int cx;
    int rowoff;
    int coloff;
} window;

typedef struct {
    window* windows;
    int scrrows;
    int scrcols;
} screen;

window newWin(int, int);
void freeWin(window*);
void drawWin(window*);
void drawBuf(window*, buffer*);

#endif
