#ifndef FEARS_H
#define FEARS_H

#define GLOB(x) ((void)x)
#define DIE(msg, ...)                        \
    do {                                     \
        fprintf(stderr, msg, __VA_ARGS__); \
        exit(1);                             \
    } while (0)

#define BUFS 48

typedef enum {
    MODE_NORMAL,
    MODE_INSERT
} EditorMode;

typedef struct {
    int cx;          // cursor x
    int cy;          // cursor y
    int rowoff;      // the index of the first line in your buffer that is currently displayed at the top of the screen
    int winrows;     // window rows
    int wincols;     // window columns
    int curBuf;      // current buffer number
    EditorMode mode; // editor mode
} EditorState;

extern char* filenames[];

#endif // FEARS_H
