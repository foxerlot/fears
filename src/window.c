#include <ncurses.h>
#include <stdlib.h>
#include "window.h"

void draw(window* win, buffer* buf)
{
    if (!win || !buf) return;

    int vRow, vCol; // visual row and column

    move(0, 0);
    for (vRow = 0; vRow < win->winrows; vRow++) {
        for (vCol = 0; vCol < win->wincols; vCol++) {
            printw("%c", buf->rows[vRow].line[vCol]);
        }
        move(vRow, 1);
    }

    refresh();
}

window* newWin(int rows, int cols)
{
    window* win = malloc(sizeof(window));
    win->winrows = rows;
    win->wincols = cols;
    return win;
}
