#ifndef SCREEN_H
#define SCREEN_H

#include "buffer.h"
#include "editor.h"

void screen_init(EditorState*);
void screen_draw(buffer*, int cx, int cy, int rowoff, int winrows);
void editor_cleanup(EditorState*, int);

#endif