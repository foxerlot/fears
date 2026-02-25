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

extern int cx, cy;
extern int rowoff;
extern int winrows, wincols;
extern char* current_filename;

buffer* fileToBuf(const char*);
void draw(buffer*);
void insertChar(row*, int, char);
void deleteChar(row*, int);
void insertNewline(buffer*, int, int);
void saveBuf(buffer*);

#endif
