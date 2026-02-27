#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "buffer.h"
#include "tui.h"
#include "fears.h"

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
    .curbuf = 0,
    .mode = MODE_NORMAL,
};

buffer* buflist[BUFS] = {NULL};

void parseArgs(int, char**);
void editorInit(void);
void editorLoop(void);
void editorCleanup(int);

int main(int argc, char** argv)
{
    parseArgs(argc, argv);
    editorInit();
    editorLoop();
    editorCleanup(EXIT_SUCCESS);
    return 0;
}

void parseArgs(int argc, char** argv)
{
    int k = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--version") == 0) {
            printf("fears version 0.1\n");
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: fears [options] [file]\n");
            printf("Options:\n");
            printf("\t--version\tDisplay version number\n");
            printf("\t--help\tDisplay help information\n"); // TODO: implement help option
            exit(EXIT_SUCCESS);
        } else {
            FILE* f = fopen(argv[i], "r");
            if (!f) exit(EXIT_FAILURE);
            buflist[k++] = fileToBuf(f);
            fclose(f);
        }
    }
}

void editorInit(void)
{
    enterRawMode();
    enableAltBuffer();
    clear();
    getmaxyx(&state.winrows, &state.wincols);
    draw(buflist[state.curbuf]);
}

void editorLoop(void) {
    int ch;
    while (1) {
        ch = readKey();

        if (state.mode == MODE_NORMAL) {
            switch (ch) {
                case 'q':
                    return;
                    break;
                case 'i':
                    state.mode = MODE_INSERT;
                    break;
                case 'h':
                    if (state.cx > 0) state.cx--;
                    break;
                case 'j':
                    if (state.cy < buflist[state.curbuf]->numrows - 1) state.cy++;
                    break;
                case 'k':
                    if (state.cy > 0) state.cy--;
                    break;
                case 'l':
                    if (state.cx < buflist[state.curbuf]->rows[state.cy].length) state.cx++;
                    break;
                case 'b':
                    if (buflist[state.curbuf+1] != NULL)
                        state.curbuf++;
                    else
                        state.curbuf = 0;
                    break;
                case ARROW_UP:
                    if (state.cy > 0) state.cy--;
                    break;
                case ARROW_DOWN:
                    if (state.cy < buflist[state.curbuf]->numrows - 1) state.cy++;
                    break;
                case ARROW_LEFT:
                    if (state.cx > 0) state.cx--;
                    break;
                case ARROW_RIGHT:
                    if (state.cx < buflist[state.curbuf]->rows[state.cy].length) state.cx++;
                    break;
            }
        }
        else if (state.mode == MODE_INSERT) {
            if (ch == ESC) {
                state.mode = MODE_NORMAL;
            } else if (ch >= CHAR_START && ch <= CHAR_END) {
                insertChar(&buflist[state.curbuf]->rows[state.cy], state.cx, ch);
                state.cx++;
            } else if (ch == BACKSPACE || ch == DEL) {
                deleteChar(buflist[state.curbuf], state.cy, state.cx-1);
                state.cx--;
            } else if (ch == CR || ch == LF) {
                insertCR(buflist[state.curbuf], state.cy, state.cx);
                state.cy++;
                state.cx = 0;
            }
        }
        draw(buflist[state.curbuf]);
    }
}

void editorCleanup(int statusc) // statusc = status code
{
    freeBuf(buflist[state.curbuf]);
    disableAltBuffer();
    exit(statusc);
}
