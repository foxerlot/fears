#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

void buffer_load_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return;

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, f)) != -1) {
        if (buf.numrows == buf.capacity) {
            buf.capacity = buf.capacity ? buf.capacity * 2 : 16;
            buf.rows = realloc(buf.rows, buf.capacity * sizeof(row));
        }
        buf.rows[buf.numrows].line = malloc(nread + 1);
        strcpy(buf.rows[buf.numrows].line, line);
        buf.rows[buf.numrows].length = nread;
        buf.numrows++;
    }

    free(line);
    fclose(f);
}

void draw(void) {
    clear();
    for (int i = 0; i < winrows && i + rowoff < buf.numrows; i++) {
        mvprintw(i, 0, "%s", buf.rows[i + rowoff].line);
    }
    move(cy - rowoff, cx);
    refresh();
}

void row_insert_char(row* r, int at, char c) {
    if (at < 0 || at > r->length) return;
    r->line = realloc(r->line, r->length + 2);
    memmove(&r->line[at + 1], &r->line[at], r->length - at + 1); // +1 for '\0'
    r->line[at] = c;
    r->length++;
}

void row_delete_char(row* r, int at) {
    if (at < 0 || at >= r->length) return;
    memmove(&r->line[at], &r->line[at + 1], r->length - at);
    r->length--;
    r->line = realloc(r->line, r->length + 1);
}
