#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include "buffer.h"
#include "editor.h"
#include "window.h"

#ifdef WIN32
#   include <io.h>
#   define F_OK 0
#   define access _access
#else
#   include <unistd.h>
#endif

int bufcount = 0;
buffer* buflist[8] = {NULL};

void parseArgs(int, char**);
void editorInit(void);
void editorCleanup(int);

int main(int argc, char** argv)
{
    parseArgs(argc, argv);
    editorInit();
    editorCleanup(EXIT_SUCCESS);
    return 0;
}

void parseArgs(int argc, char** argv)
{
    if (argc != 2) editorCleanup(EXIT_FAILURE);
    FILE* f = fopen(argv[1], "r");
    if (!f) editorCleanup(EXIT_FAILURE);

    buflist[bufcount++] = fileToBuf(f);
    fclose(f);
}

void editorInit(void)
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    draw(buflist[state.curbuf]);
}

void editorCleanup(int statusc) // statusc = status code
{
    endwin();
    freeBuf(buflist[state.curbuf]);
    exit(statusc);
}
