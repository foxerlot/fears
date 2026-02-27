#ifndef EDITOR_H
#define EDITOR_H

typedef enum {
    NORMAL_MODE,
    INSERT_MODE,
    COMMAND_MODE,
} EditorMode;

typedef struct {
    int cx;
    int cy;
    int rowoff;
    int scrrows;
    int scrcols;
    int curbuf;
    EditorMode mode;
} EditorState;

extern EditorState state;

#endif
