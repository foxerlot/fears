#include <ncurses.h>
#include <stdlib.h>
#include "editor.h"
#include "buffer.h"
#include "window.h"

EditorState state = {
    .cx = 0,
    .cy = 0,
    .rowoff = 0,
    .scrrows = 0,
    .scrcols = 0,
    .curbuf = 0,
    .mode = NORMAL_MODE,
};

buffer* buflist[8] = {NULL};

void editorInit(void)
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    window* win = newWin(state.scrrows, state.scrcols); // ideally this would be handled by parseArgs in main.c
    draw(win, buflist[state.curbuf]);
}

void editorCleanup(int statusc) // statusc = status code
{
    endwin();
    freeBuf(buflist[state.curbuf]);
    exit(statusc);
}
