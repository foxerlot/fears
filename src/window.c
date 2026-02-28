#include <ncurses.h>
#include "window.h"

/* usage should look something like this:
 * This is still a work-in-progress library...
int main(void)
{
    initscr(); // ncurses function

    screen* tempScr = newScreen();
    window* mainWin = newWin(tempScr->scrrows, tempScr->scrcols);
    addWin(tempScr, mainWin);
    drawScreen(tempScr);

    freeWin(mainWin); // was allocated in newWin()
    freeScreen(tempScr); // was allocated in newScreen();

    endwin(); // ncurses function
    return 0; // obligitory main return
}
 */

void drawScreen(screen* screen)
{
    if (!screen) return;

    for (int i = 0; i < screen->numwins; i++) {
        drawWin(&screen->windows[i]);
    }
}

void drawWin(window* win)
{
    if (!win) return;

    int vRows, vCols;
    for (vRows = 0; vRows < win->buf->rows[vRows].length && vRows < win->winrows; vRows++) {
        for (vCols = 0; vCols < win->wincols; vCols++) {
            printw("%c", win->buf->rows[vRows].line[vCols]);
        }
    }
}
