#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ncurses.h>
#include "fears.h"
#include "keys.h"
#include "buffer.h"

#ifdef WIN32
#   include <io.h>
#   define F_OK 0
#   define access _access
#else
#   include <unistd.h>
#endif

EditorState state = {
    .cx = 0,
    .cy = 0,
    .rowoff = 0,
    .winrows = 0,
    .wincols = 0,
    .curBuf = 0,
    .mode = MODE_NORMAL
};

buffer* buflist[BUFS];
char usage[] = "Usage: fears [options] [filename]";
int bufcount = 0;
char* filenames[BUFS];

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
    if (argc < 2) DIE("%s", usage);

    for (int i = 1; i < argc; i++) {
        // if (isfile(argv[0])) // TODO
        filenames[i - 1] = argv[i];
    }

    bufcount = argc - 1;
    for (int i = 0; i < bufcount; i++) {
        if (access(argv[i + 1], F_OK) == 0)
            buflist[i] = fileToBuf(filenames[i]);
        else
            buflist[i] = newBuffer();
        if (!buflist[i]) DIE("Error: could not open file %s\n", filenames[i]);
    }
}

void editor_init(void)
{
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, state.winrows, state.wincols);
    draw(buflist[state.curBuf]);
}

void editor_loop(void) // eventually put this in an input.c
{
    int ch;
    while (true) {
        ch = getch();
        if (state.mode == MODE_NORMAL){
            switch (ch) { // qwertyuiopasdfghjklzxcvbnm
            case 'q':
                return;
                break;
            case 'w':
                saveBuf(buflist[state.curBuf]);
                break;
            case 'i':
                state.mode = MODE_INSERT;
                break;
            case 'h':
                if (state.cx > 0) state.cx--;
                break;
            case 'j':
                if (state.cy < buflist[state.curBuf]->numrows - 1) state.cy++;
                if (state.cy >= state.rowoff + state.winrows) state.rowoff = state.cy - state.winrows + 1;
                break;
            case 'k':
                if (state.cy > 0) state.cy--;
                if (state.cy < state.rowoff) state.rowoff = state.cy;
                break;
            case 'l':
                if (state.cx < buflist[state.curBuf]->rows[state.cy].length) state.cx++;
                break;
            case 'b':
                if (buflist[state.curBuf+1] != NULL)
                    state.curBuf++;
                else
                    state.curBuf = 0;
                break;
            }
        } else if (state.mode == MODE_INSERT) {
            if (ch == ESC) {
                state.mode = MODE_NORMAL;
            } else if (ch >= CHAR_START && ch <= CHAR_END) {
                insertChar(&buflist[state.curBuf]->rows[state.cy], state.cx, ch);
                state.cx++;
            } else if (ch == KEY_BACKSPACE || ch == DEL) {
                deleteChar(&buflist[state.curBuf]->rows[state.cy], state.cx-1);
                state.cx--;
            } else if (ch == KEY_ENTER || ch == CR) {
                insertNewline(buflist[state.curBuf], state.cy, state.cx);
                state.cy++;
                state.cx = 0;
            }
        }
        draw(buflist[state.curBuf]);
    }
}

void editor_cleanup(int statusc) // statusc = status code
{
    endwin();
    for (int i = 0; i < bufcount; i++) {
        for (int j = 0; j < buflist[i]->numrows; j++)
            free(buflist[i]->rows[j].line);
        free(buflist[i]->rows);
        free(buflist[i]);
    }
    exit(statusc);
}
