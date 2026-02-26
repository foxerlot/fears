#include "cli.h"
#include "editor.h"
#include "input.h"
#include "screen.h"

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
    screen_init(&state);
    editor_loop(&state);
    editor_cleanup(&state, 0);
    return 0;
}