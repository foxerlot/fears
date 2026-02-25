#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "buffer.h"

#define GLOB(x) ((void)x)

typedef enum {
    MODE_NORMAL,
    MODE_INSERT
} EditorMode;

EditorMode mode = MODE_NORMAL;
buffer buf = {0, 0, NULL};

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
        fprintf(stderr, "%s", usage);
        exit(0);
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

// if (argc != 2) return 1; // TODO: implement a better argument parsing system
// buffer_load_file(argv[1]);

void editor_loop(void)
{
    while (true) {
        draw();
    }
}

void editor_cleanup(int statusc)
{
    endwin();
    for (int i = 0; i < buf.numrows; i++)
        free(buf.rows[i].line);
    free(buf.rows);
    exit(statusc);
}
