#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "cli.h"
#include "editor.h"
#include "input.h"
#include "screen.h"

EditorState editor_open(int argc, char** argv)
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

    const char* filename = parse_args(argc, argv);
    state.buflist[state.curBuf] = fileToBuf(filename);
    if (!state.buflist[state.curBuf]) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    return state;
}

void editor_init(EditorState* state)
{
    screen_init(&state->winrows, &state->wincols);
}

void editor_run(EditorState* state)
{
    input_loop(state);
}

void editor_cleanup(EditorState* state, int statusc)
{
    for (int i = 0; i < MAX_BUFFERS; i++)
        if (state->buflist[i])
            freeBuf(state->buflist[i]);
    screen_cleanup();
    exit(statusc);
}