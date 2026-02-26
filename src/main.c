#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "buffer.h"
#include "editor.h"
#include "input.h"
#include "keys.h"

static char* usage = "Usage: fears [options] [input]";

int main(int argc, char** argv)
{
    EditorState state = {
        .mode    = MODE_NORMAL,
        .curBuf  = 0,
        .cx      = 0,
        .cy      = 0,
        .rowoff  = 0,
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