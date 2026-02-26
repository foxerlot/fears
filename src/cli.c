#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "cli.h"
#include "editor.h"

static char* usage = "Usage: fears [options] [input]";

void parse_args(EditorState* state, int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "%s\n", usage);
        exit(1);
    }

    // fileToBuf now always returns a valid buffer — new file or existing
    state->buflist[state->curBuf] = fileToBuf(argv[1]);
    if (!state->buflist[state->curBuf]) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
}