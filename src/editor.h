#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#define MAX_BUFFERS 8

typedef enum {
    MODE_NORMAL,
    MODE_INSERT
} EditorMode;

typedef struct {
    EditorMode mode;
    buffer* buflist[MAX_BUFFERS];
    int curBuf;
    int cx, cy;
    int rowoff;
    int winrows, wincols;
} EditorState;

void parse_args(EditorState*, int, char**);
void editor_init(EditorState*);
void editor_loop(EditorState*);
void editor_cleanup(EditorState*, int);

#endif