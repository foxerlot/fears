#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "buffer.h"
#include "keys.h"

#define GLOB(x) ((void)x)

typedef enum {
    MODE_NORMAL,
    MODE_INSERT
} EditorMode;

EditorMode mode = MODE_NORMAL;
buffer* buflist[8];
int curBuf = 0;

int cx = 0, cy = 0;    // cursor y, cursor x
int rowoff = 0;        // the index of the first line in your buffer that is currently displayed at the top of the screen
int winrows, wincols;  // window rows, window columns

char* usage = "Usage: fears [options] [input]";

void parse_args(int, char**);
void editor_init(void);
void editor_loop(void);
void editor_cleanup(int);

int main(int argc, char** argv)
{
    parse_args(argc, argv);
    editor_init();
    editor_loop();
    editor_cleanup(0);
    return 0;
}

void parse_args(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "%s\n", usage);
        exit(1);
    }
    buflist[curBuf] = fileToBuf(argv[1]);
    if (!buflist[curBuf]) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        exit(1);
    }
}

void editor_init(void)
{
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, winrows, wincols);
}

void editor_loop(void) // eventually put this in an input.c
{
    int ch;
    while (true) {
        ch = getch();
        if (mode == MODE_NORMAL){
            switch (ch) { // qwertyuiopasdfghjklzxcvbnm
            case 'q':
                return;
                break;
            case 'w':
                saveBuf(buflist[curBuf]);
                break;
            case 'i':
                mode = MODE_INSERT;
                break;
            case 'h':
                if (cx > 0) cx--;
                break;
            case 'j':
                if (cy < buflist[curBuf]->numrows - 1) cy++;
                if (cy > rowoff + winrows) rowoff = cy - winrows + 1;
                break;
            case 'k':
                if (cy > 0) cy--;
                if (cy < rowoff) rowoff = cy;
                break;
            case 'l':
                if (cx < buflist[curBuf]->rows[cy].length) cx++;
                break;
            }
        } else if (mode == MODE_INSERT) {
            if (ch == ESC) {
                mode = MODE_NORMAL;
            } else if (ch >= CHAR_START && ch <= CHAR_END) {
                insertChar(&buflist[curBuf]->rows[cy], cx, ch);
                cx++;
            } else if (ch == KEY_BACKSPACE || ch == DEL) {
                deleteChar(&buflist[curBuf]->rows[cy], cx-1);
                cx--;
            } else if (ch == KEY_ENTER || ch == CR) {
                insertNewline(buflist[curBuf], cy, cx);
                cy++;
                cx = 0;
            }
        }
        draw(buflist[curBuf]);
    }
}

void editor_cleanup(int statusc) // statusc = status code
{
    endwin();
    for (int i = 0; i < 8; i++) { // 8 is the size of buflist, prob make this a macro...
        for (int j = 0; j < buflist[i]->numrows; j++)
            free(buflist[i]->rows[j].line);
        free(buflist[i]->rows);
        free(buflist[i]);
    }
    exit(statusc);
}
