#ifndef BUFFER_H
#define BUFFER_H

#include "fears.h"

typedef struct {
    int length;
    char* line;
} row;

typedef struct {
    int numrows;
    int capacity;
    row *rows;
} buffer;

extern EditorState state;

buffer* fileToBuf(const char*);
buffer* newBuffer(void);
void draw(buffer*);
void insertChar(row*, int, char);
void deleteChar(row*, int);
void insertNewline(buffer*, int, int);
void saveBuf(buffer*);

#endif
