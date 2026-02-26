#include <ncurses.h>
#include <stdbool.h>
#include "buffer.h"
#include "editor.h"
#include "input.h"
#include "keys.h"

void editor_loop(EditorState* state)
{
    int ch;

    draw(state->buflist[state->curBuf],
         state->cx, state->cy, state->rowoff, state->winrows);

    while (true) {

        ch = getch();

        buffer* buf    = state->buflist[state->curBuf];
        row*    currow = &buf->rows[state->cy];

        if (state->mode == MODE_NORMAL) {

            switch (ch) {

            case 'q':
                return;

            case 'w':
                saveBuf(buf);
                break;

            case 'i':
                state->mode = MODE_INSERT;
                break;

            case KEY_LEFT:
            case 'h':
                if (state->cx > 0) {
                    state->cx--;
                } else if (state->cy > 0) {
                    state->cy--;
                    state->cx = buf->rows[state->cy].length;
                }
                break;

            case KEY_RIGHT:
            case 'l':
                if (state->cx < currow->length) {
                    state->cx++;
                } else if (state->cy < buf->numrows - 1) {
                    state->cy++;
                    state->cx = 0;
                }
                break;

            case KEY_UP:
            case 'k':
                if (state->cy > 0)
                    state->cy--;
                break;

            case KEY_DOWN:
            case 'j':
                if (state->cy < buf->numrows - 1)
                    state->cy++;
                break;

            case KEY_BACKSPACE:
            case 127:
                if (state->cx > 0) {
                    deleteChar(currow, state->cx - 1);
                    state->cx--;
                }
                break;

            case KEY_DC:
                if (state->cx < currow->length)
                    deleteChar(currow, state->cx);
                break;
            }

        } else if (state->mode == MODE_INSERT) {

            if (ch == ESC) {

                state->mode = MODE_NORMAL;

            } else if (ch == KEY_BACKSPACE || ch == 127) {

                if (state->cx > 0) {
                    deleteChar(currow, state->cx - 1);
                    state->cx--;
                }

            } else if (ch == KEY_DC) {

                if (state->cx < currow->length)
                    deleteChar(currow, state->cx);

            } else if (ch == KEY_ENTER || ch == '\n' || ch == '\r') {

                insertNewline(buf, state->cy, state->cx);
                state->cy++;
                state->cx = 0;

            } else if (ch >= CHAR_START && ch <= CHAR_END) {

                insertChar(currow, state->cx, ch);
                state->cx++;
            }
        }

        // Fix cursor bounds after movement
        if (state->cy >= buf->numrows)
            state->cy = buf->numrows - 1;

        if (state->cy < 0)
            state->cy = 0;

        if (state->cx > buf->rows[state->cy].length)
            state->cx = buf->rows[state->cy].length;

        if (state->cx < 0)
            state->cx = 0;

        // Scroll handling
        if (state->cy < state->rowoff)
            state->rowoff = state->cy;

        if (state->cy >= state->rowoff + state->winrows)
            state->rowoff = state->cy - state->winrows + 1;

        draw(buf, state->cx, state->cy, state->rowoff, state->winrows);
    }
}