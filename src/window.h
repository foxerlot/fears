#ifndef WINDOW_H
#define WINDOW_H

#include "buffer.h"

typedef struct {
    int winrows;
    int wincols;
} window;

void draw(window*, buffer*);
window* newWin(int, int);

#endif
