#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "win_getline.h"

char* current_filename = NULL;

void buffer_load_file(const char* filename) {
    current_filename = (char*) filename;
    FILE* f = fopen(filename, "r");
    if (!f) return;

    char* line = NULL;
    size_t len = 0;
    size_t nread;

    while ((nread = win_getline(&line, &len, f)) != -1) {
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

void row_split(buffer *buf, int row_index, int at)
{
    row *r = &buf->rows[row_index];

    if (at < 0 || at > r->length) return;

    // Create new row
    row new_row;
    new_row.length = r->length - at;
    new_row.line = malloc(new_row.length + 1);
    memcpy(new_row.line, &r->line[at], new_row.length);
    new_row.line[new_row.length] = '\0';

    r->length = at;
    r->line[at] = '\0';

    if (buf->numrows == buf->capacity) {
        buf->capacity = buf->capacity ? buf->capacity * 2 : 8;
        buf->rows = realloc(buf->rows, buf->capacity * sizeof(row));
    }

    memmove(&buf->rows[row_index + 2],
            &buf->rows[row_index + 1],
            (buf->numrows - row_index - 1) * sizeof(row));

    buf->rows[row_index + 1] = new_row;
    buf->numrows++;
}

/* void row_insert_newline(row* row, int at) { //////////////////////////this don't work kek
    if (at < 0 || at > row->size) return;
    row* new_row = malloc(sizeof(row));
    new_row->size = row->size - at;
    new_row->chars = malloc(new_row->size);
    memcpy(new_row->chars, row->chars + at, new_row->size);
    row->size = at;
    row->chars = realloc(row->chars, row->size + 1);
    row->chars[row->size] = '\n';
    buf.numrows++;
    buf.rows = realloc(buf.rows, buf.numrows * sizeof(row));
    memmove(&buf.rows[cy + 1], &buf.rows[cy], (buf.numrows - cy - 1) * sizeof(row));
    buf.rows[cy] = *new_row; } */

int buffer_save(void)
{
    if (!current_filename) return -1;

    FILE *fp = fopen(current_filename, "w");
    if (!fp) return -1;

    for (int i = 0; i < buf.numrows; i++) {
        fwrite(buf.rows[i].line, 1, buf.rows[i].length, fp);
        fputc('\n', fp);  // add newline when saving
    }

    fclose(fp);
    return 0;
}
