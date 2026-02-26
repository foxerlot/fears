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
        // File doesn't exist — create a new empty buffer with that filename
        current_filename = strdup(argv[1]);
        buflist[curBuf] = malloc(sizeof(buffer));
        if (!buflist[curBuf]) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        buflist[curBuf]->numrows = 1;
        buflist[curBuf]->capacity = 1;
        buflist[curBuf]->rows = malloc(sizeof(row));
        buflist[curBuf]->rows[0].length = 0;
        buflist[curBuf]->rows[0].line = malloc(1);
        buflist[curBuf]->rows[0].line[0] = '\0';
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

void editor_loop(void)
{
    int ch;

    draw(buflist[curBuf]);

    while (true) {

        ch = getch();

        buffer* buf = buflist[curBuf];
        row* currow = &buf->rows[cy];

        if (mode == MODE_NORMAL) {

            switch (ch) {

            case 'q':
                return;

            case 'w':
                saveBuf(buf);
                break;

            case 'i':
                mode = MODE_INSERT;
                break;

            // Arrow keys
            case KEY_LEFT:
            case 'h':
                if (cx > 0) {
                    cx--;
                } else if (cy > 0) {
                    cy--;
                    cx = buf->rows[cy].length;
                }
                break;

            case KEY_RIGHT:
            case 'l':
                if (cx < currow->length) {
                    cx++;
                } else if (cy < buf->numrows - 1) {
                    cy++;
                    cx = 0;
                }
                break;

            case KEY_UP:
            case 'k':
                if (cy > 0)
                    cy--;
                break;

            case KEY_DOWN:
            case 'j':
                if (cy < buf->numrows - 1)
                    cy++;
                break;

            case KEY_BACKSPACE:
            case 127:
                if (cx > 0) {
                    deleteChar(currow, cx - 1);
                    cx--;
                }
                break;

            case KEY_DC:  // Delete key
                if (cx < currow->length) {
                    deleteChar(currow, cx);
                }
                break;
            }

        } else if (mode == MODE_INSERT) {

            if (ch == ESC) {

                mode = MODE_NORMAL;

            } else if (ch == KEY_BACKSPACE || ch == 127) {

                if (cx > 0) {
                    deleteChar(currow, cx - 1);
                    cx--;
                }

            } else if (ch == KEY_DC) {

                if (cx < currow->length) {
                    deleteChar(currow, cx);
                }

            } else if (ch == KEY_ENTER || ch == '\n' || ch == '\r') {

                insertNewline(buf, cy, cx);
                cy++;
                cx = 0;

            } else if (ch >= CHAR_START && ch <= CHAR_END) {

                insertChar(currow, cx, ch);
                cx++;
            }
        }

        // Fix cursor bounds after movement
        if (cy >= buf->numrows)
            cy = buf->numrows - 1;

        if (cy < 0)
            cy = 0;

        if (cx > buf->rows[cy].length)
            cx = buf->rows[cy].length;

        if (cx < 0)
            cx = 0;

        // Scroll handling
        if (cy < rowoff)
            rowoff = cy;

        if (cy >= rowoff + winrows)
            rowoff = cy - winrows + 1;

        draw(buf);
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
    free(current_filename);
    exit(statusc);
}
