#ifndef SCREEN_H
#define SCREEN_H

#include "buffer.h"

// screen.c owns all ncurses interaction.
// It takes only primitive values — no EditorState dependency.

void screen_init(int* winrows, int* wincols);
void screen_draw(buffer*, int cx, int cy, int rowoff, int winrows);
void screen_cleanup(void);

#endif