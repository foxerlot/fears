#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    int length;
    char* line;
} row;

typedef struct {
    int numrows;
    int capacity;
    row *rows;
} buffer;

extern buffer buf;
extern int cx, cy;
extern int rowoff;
extern int winrows, wincols;
extern char* current_filename;

void buffer_load_file(const char*);
void draw(void);
void row_insert_char(row*, int, char);
void row_delete_char(row*, int);
void row_split(buffer*, int, int);
int buffer_save(void);

#endif
