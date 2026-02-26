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
    char* filename;
} buffer;

buffer* fileToBuf(const char*);
void draw(buffer*, int cx, int cy, int rowoff, int winrows);
void insertChar(row*, int, char);
void deleteChar(row*, int);
void insertNewline(buffer*, int, int);
void saveBuf(buffer*);

#endif