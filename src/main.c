#include "editor.h"

int main(int argc, char** argv)
{
    EditorState state = editor_open(argc, argv);
    editor_run(&state);
    editor_cleanup(&state, 0);
    return 0;
}