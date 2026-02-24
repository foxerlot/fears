#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "keys.h"

typedef enum {
    MODE_NORMAL,
    MODE_INSERT
} EditorMode;

EditorMode mode = MODE_NORMAL;
buffer buf = {0, 0, NULL};

int cx = 0, cy = 0;    // cursor y, cursor x
int rowoff = 0;        // the index of the first line in your buffer that is currently displayed at the top of the screen
int winrows, wincols;  // window rows, window columns

void editor_loop(void)
{
    int ch;
    while ((ch = getch()) != 3)
    {
        if (mode == MODE_NORMAL) {
            switch (ch) { // qwertyuiopasdfghjklzxcvbnm
            case 'q': // implement this as a ex command
                exit(0);
                break;
            case 'w': // implement this as a ex command
                buffer_save();
                break;
            case 'i': // insert
                mode = MODE_INSERT;
                break;
            case 'h': // left
                if (cx > 0) cx--;
                break;
            case 'j': // down
                if (cy < buf.numrows - 1) cy++;
                if (cy > rowoff + winrows) rowoff++;
                if (cx > buf.rows[cy].length) cx = buf.rows[cy].length;
                break;
            case 'k': // up
                if (cy > 0) cy--;
                if (cy < rowoff) rowoff--;
                if (cx > buf.rows[cy].length) cx = buf.rows[cy].length;
                break;
            case 'l': //right
                if (cx < buf.rows[cy].length) cx++;
                break;
            case CTRL_D:
                if (cy < buf.numrows + (winrows / 2) - 1) cy += winrows / 2;
                if (cy > rowoff + winrows) rowoff += winrows / 2;
                if (cy > buf.numrows) cy = buf.numrows;
            }
        } else if (mode == MODE_INSERT) {
            if (ch == ESC) { // normal
                mode = MODE_NORMAL;
            } else if (ch >= CHAR_START && ch <= CHAR_END) { // insert char
                row_insert_char(&buf.rows[cy], cx, ch);
                cx++;
            } else if (ch == KEY_BACKSPACE || ch == DEL) { // backspace
                if (cx > 0) {
                    row_delete_char(&buf.rows[cy], cx - 1);
                    cx--;
                }
            } else if (ch == '\n' || ch == KEY_ENTER) { // newline handling
                row_split(&buf, cy, cx);
                cy++;
                cx = 0;
            }
        }
        draw();
    }
}

int main(int argc, char** argv)
{
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
