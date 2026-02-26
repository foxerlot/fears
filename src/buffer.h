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
    char* filename;   // moved from global current_filename in buffer.c
} buffer;

extern int cx, cy;
extern int rowoff;
extern int winrows, wincols;

buffer* fileToBuf(const char*);
void draw(buffer*);
void insertChar(row*, int, char);
void deleteChar(row*, int);
void insertNewline(buffer*, int, int);
void saveBuf(buffer*);

#endif