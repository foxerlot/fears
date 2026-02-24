#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int length;
    char* line;
} row;

typedef struct {
    int numrows;
    int capacity;
    row *rows;
} buffer;

buffer buf = {0, 0, NULL};
int bufpos = 0;        // scroll offset
int winrows, wincols;  // terminal size

void draw(void) {
    clear();
    for (int i = 0; i < winrows && i + bufpos < buf.numrows; i++) {
        mvprintw(i, 0, "%s", buf.rows[i + bufpos].line);
    }
    refresh();
}

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    FILE* contents = fopen(argv[1], "r");
    if (!contents) return 1;

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, contents)) != -1) {
        // grow buffer if needed
        if (buf.numrows == buf.capacity) {
            buf.capacity = buf.capacity ? buf.capacity * 2 : 16;
            buf.rows = realloc(buf.rows, buf.capacity * sizeof(row));
        }

        // store line
        buf.rows[buf.numrows].line = malloc(nread + 1);
        strcpy(buf.rows[buf.numrows].line, line);
        buf.rows[buf.numrows].length = nread;
        buf.numrows++;
    }

    free(line);
    fclose(contents);

    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    getmaxyx(stdscr, winrows, wincols);

    draw();

    int ch;
    while ((ch = getch()) != 3) {
        int curbufpos = bufpos;
        switch (ch) {
        case KEY_DOWN:
            if (bufpos < buf.numrows - winrows)
                bufpos++;
            break;
        case KEY_UP:
            if (bufpos > 0)
                bufpos--;
            break;
        }
        if (bufpos != curbufpos) draw();
    }

    endwin();

    for (int i = 0; i < buf.numrows; i++)
        free(buf.rows[i].line);
    free(buf.rows);

    return 0;
}
