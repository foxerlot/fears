#include <ncurses.h>
#include "buffer.h"
#include "screen.h"

void screen_init(int* winrows, int* wincols)
{
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, *winrows, *wincols);
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

void screen_cleanup(void)
{
    endwin();
}