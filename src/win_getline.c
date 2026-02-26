#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "win_getline.h"

// Returns the number of characters read, or -1 on EOF or error.
// Uses ptrdiff_t instead of ssize_t because ssize_t is a POSIX type
// not available on Windows (MSVC/MinGW). ptrdiff_t is standard C99
// and safely holds both positive sizes and the -1 sentinel value.
ptrdiff_t win_getline(char **lineptr, size_t *n, FILE *stream)
{
    if (!lineptr || !n || !stream)
        return -1;

    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = (char *)malloc(*n);
        if (!*lineptr)
            return -1;
    }

    size_t pos = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {

        if (pos + 1 >= *n) {
            size_t new_size = (*n) * 2;
            char *new_ptr = (char *)realloc(*lineptr, new_size);
            if (!new_ptr)
                return -1;
            *lineptr = new_ptr;
            *n = new_size;
        }

        (*lineptr)[pos++] = (char)c;

        if (c == '\n')
            break;
    }

    if (pos == 0 && c == EOF)
        return -1;

    (*lineptr)[pos] = '\0';
    return (ptrdiff_t)pos;
}