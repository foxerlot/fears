#ifndef FEARS_H
#define FEARS_H

#define BUFS 48

typedef enum {
    MODE_NORMAL,
    MODE_INSERT
} EditorMode;

typedef struct {
    int cx;           // cursor x
    int cy;           // cursor y
    int rowoff;       // the index of the first line in your buffer that is currently displayed at the top of the screen
    int winrows;      // window rows
    int wincols;      // window columns
    int curbuf;       // current buffer number
    EditorMode mode;  // editor mode
} EditorState;

extern EditorState state;

#endif // FEARS_H
