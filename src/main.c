#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int winrows, wincols, bufpos, lines = 0;
char** buffer = NULL;

void draw(void);

int main(int argc, char** argv)
{
    if (argc != 2) return 1;
    FILE* contents = fopen(argv[1], "r");
    if (!contents) return 1;

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    while((nread = getline(&line, &len, contents)) != -1) {
        buffer = realloc(buffer, (lines + 1) * sizeof(char*));
        buffer[lines] = malloc(nread + 1);
        strcpy(buffer[lines], line);
        lines++;
    }
    free(line);
    fclose(contents);

    initscr();
    noecho();
    raw();
    getmaxyx(stdscr, winrows, wincols);
    draw();

    int ch = 0;
    while (true) { // qwertyuiopasdfghjklzxcvbnm
        int curbufpos = bufpos;
        ch = getch();
        switch(ch) {
        case 'q':
            goto exit;
            break;
        case 'j':
            if (bufpos < lines - 1)
                bufpos++;
            break;
        case 'k':
            if (bufpos > 0)
                bufpos--;
            break;
        }
        if (bufpos != curbufpos) draw();
    }

exit:

    endwin();
    for (int i = 0; i < lines; i++)
        free(buffer[i]);
    free(buffer);

    return 0;
}

void draw(void)
{
    clear();

    for (int i = 0, j = bufpos;
         i < winrows && j < lines;
         i++, j++)
    {
        mvprintw(i, 0, "%s", buffer[j]);
    }

    refresh();
}
