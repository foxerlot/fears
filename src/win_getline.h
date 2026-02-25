#ifndef WIN_GETLINE_H
#define WIN_GETLINE_H

#include <stdio.h>
#include <stddef.h>

ptrdiff_t win_getline(char**, size_t*, FILE*); // -1 on EOF or error

#endif
