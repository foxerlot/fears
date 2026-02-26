#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "win_getline.h"

buffer* fileToBuf(const char* filename)
{
    buffer* buf = malloc(sizeof(buffer));
    if (!buf) return NULL;
    buf->numrows = 0;
    buf->capacity = 0;
    buf->rows = NULL;
    buf->filename = strdup(filename);

    FILE* f = fopen(filename, "r");
    if (!f) {
        // File doesn't exist — return an empty buffer with the filename preserved
        buf->capacity = 1;
        buf->rows = malloc(sizeof(row));
        buf->rows[0].length = 0;
        buf->rows[0].line = malloc(1);
        buf->rows[0].line[0] = '\0';
        buf->numrows = 1;
        return buf;
    }

    char* line = NULL;
    size_t len = 0;
    int nread;

    while ((nread = win_getline(&line, &len, f)) != -1) {
        if (nread > 0 && line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            nread--;
        }
        if (buf->numrows == buf->capacity) {
            buf->capacity = buf->capacity ? buf->capacity * 2 : 16;
            buf->rows = realloc(buf->rows, buf->capacity * sizeof(row));
        }
        buf->rows[buf->numrows].line = malloc(nread + 1);
        strcpy(buf->rows[buf->numrows].line, line);
        buf->rows[buf->numrows].length = nread;
        buf->numrows++;
    }

    free(line);
    fclose(f);

    // Empty file — ensure at least one row
    if (buf->numrows == 0) {
        buf->capacity = 1;
        buf->rows = malloc(sizeof(row));
        buf->rows[0].length = 0;
        buf->rows[0].line = malloc(1);
        buf->rows[0].line[0] = '\0';
        buf->numrows = 1;
    }
    return buf;
}

void freeBuf(buffer* buf)
{
    if (!buf) return;
    for (int i = 0; i < buf->numrows; i++)
        free(buf->rows[i].line);
    free(buf->rows);
    free(buf->filename);
    free(buf);
}

void insertChar(row* r, int at, char c)
{
    if (!r) return;
    if (at < 0) at = 0;
    if (at > r->length) at = r->length;

    r->line = realloc(r->line, r->length + 2);
    memmove(&r->line[at + 1], &r->line[at], r->length - at + 1);
    r->line[at] = c;
    r->length++;
}

void deleteChar(row* r, int at)
{
    if (!r) return;
    if (at < 0 || at >= r->length) return;
    memmove(&r->line[at],
            &r->line[at + 1],
            r->length - at);
    r->length--;
    char* tmp = realloc(r->line, r->length + 1);
    if (tmp) r->line = tmp;
}

void insertNewline(buffer* buf, int row_index, int at)
{
    if (!buf) return;
    if (row_index < 0 || row_index >= buf->numrows) return;
    row* r = &buf->rows[row_index];
    if (at < 0) at = 0;
    if (at > r->length) at = r->length;
    int right_len = r->length - at;

    row new_row;
    new_row.length = right_len;
    new_row.line = malloc(right_len + 1);
    if (!new_row.line) return;

    memcpy(new_row.line, r->line + at, right_len);
    new_row.line[right_len] = '\0';

    r->length = at;
    r->line[at] = '\0';

    if (buf->numrows == buf->capacity) {
        int new_capacity = buf->capacity ? buf->capacity * 2 : 16;
        row* tmp = realloc(buf->rows, new_capacity * sizeof(row));
        if (!tmp) {
            free(new_row.line);
            return;
        }
        buf->rows = tmp;
        buf->capacity = new_capacity;
    }

    memmove(&buf->rows[row_index + 2],
            &buf->rows[row_index + 1],
            (buf->numrows - row_index - 1) * sizeof(row));

    buf->rows[row_index + 1] = new_row;
    buf->numrows++;
}

void saveBuf(buffer* buf)
{
    if (!buf || !buf->filename) return;
    FILE* f = fopen(buf->filename, "w");
    if (!f) return;

    for (int i = 0; i < buf->numrows; i++) {
        fwrite(buf->rows[i].line, 1, buf->rows[i].length, f);
        fputc('\n', f);
    }

    fclose(f);
}