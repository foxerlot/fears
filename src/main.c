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

void editorInit(void)
{
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, winrows, wincols); // TODO
}

void editorCleanup(int statusc) // statusc = status code
{
    endwin();
    freeBuf(); // TODO
    exit(statusc);
}
