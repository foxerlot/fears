#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

buffer buf = {0, 0, NULL};
int cx = 0, cy = 0;
int rowoff = 0;
int winrows, wincols;

void editor_loop(void) {
    int ch;
    while ((ch = getch()) != 3) {
        switch(ch) {
            case KEY_UP:
                if (cy > 0) cy--;
                if (cy < rowoff) rowoff--;
                if (cx > buf.rows[cy].length) cx = buf.rows[cy].length;
                break;
            case KEY_DOWN:
                if (cy < buf.numrows - 1) cy++;
                if (cy >= rowoff + winrows) rowoff++;
                if (cx > buf.rows[cy].length) cx = buf.rows[cy].length;
                break;
            case KEY_LEFT:
                if (cx > 0) cx--;
                break;
            case KEY_RIGHT:
                if (cx < buf.rows[cy].length) cx++;
                break;
            default:
                if (ch >= 32 && ch <= 126) {
                    row_insert_char(&buf.rows[cy], cx, ch);
                    cx++;
                }
        }
        draw();
    }
}

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    buffer_load_file(argv[1]);

    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, winrows, wincols);

    draw();
    editor_loop();

    endwin();
    for (int i = 0; i < buf.numrows; i++)
        free(buf.rows[i].line);
    free(buf.rows);
    return 0;
}
