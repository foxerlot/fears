#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "buffer.h"
#include "editor.h"
#include "keys.h"

static char* usage = "Usage: fears [options] [input]";

int main(int argc, char** argv)
{
    EditorState state = {
        .mode   = MODE_NORMAL,
        .curBuf = 0,
        .cx     = 0,
        .cy     = 0,
        .rowoff = 0,
        .winrows = 0,
        .wincols = 0,
    };

    parse_args(&state, argc, argv);
    editor_init(&state);
    editor_loop(&state);
    editor_cleanup(&state, 0);
    return 0;
}

void parse_args(EditorState* state, int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "%s\n", usage);
        exit(1);
    }
    state->buflist[state->curBuf] = fileToBuf(argv[1]);
    if (!state->buflist[state->curBuf]) {
        // File doesn't exist — create a new empty buffer with that filename
        state->buflist[state->curBuf] = malloc(sizeof(buffer));
        if (!state->buflist[state->curBuf]) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        state->buflist[state->curBuf]->filename = strdup(argv[1]);
        state->buflist[state->curBuf]->numrows = 1;
        state->buflist[state->curBuf]->capacity = 1;
        state->buflist[state->curBuf]->rows = malloc(sizeof(row));
        state->buflist[state->curBuf]->rows[0].length = 0;
        state->buflist[state->curBuf]->rows[0].line = malloc(1);
        state->buflist[state->curBuf]->rows[0].line[0] = '\0';
    }
}

void editor_init(EditorState* state)
{
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, state->winrows, state->wincols);
}

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

void editor_cleanup(EditorState* state, int statusc)
{
    endwin();
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (!state->buflist[i]) continue;
        for (int j = 0; j < state->buflist[i]->numrows; j++)
            free(state->buflist[i]->rows[j].line);
        free(state->buflist[i]->rows);
        free(state->buflist[i]->filename);
        free(state->buflist[i]);
    }
    exit(statusc);
}