#include <ncurses.h>
#include "buffer.h"
#include "editor.h"
#include "screen.h"

void screen_init(EditorState* state)
{
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, state->winrows, state->wincols);
}

void screen_draw(buffer* buf, int cx, int cy, int rowoff, int winrows)
{
    clear();
    for (int i = 0; i < winrows && i + rowoff < buf->numrows; i++)
        mvprintw(i, 0, "%s", buf->rows[i + rowoff].line);
    if (cy >= rowoff && cy < rowoff + winrows)
        move(cy - rowoff, cx);
    refresh();
}