#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "tui.h"
#include "buffer.h"
#include "fears.h"

static char tui_buf[TUI_BUF_SIZE];
static size_t tui_buf_len = 0;

struct termios oldTerm, newTerm;

static void tui_buf_write(const char*, size_t);

void enterRawMode(void)
{
    tcgetattr(0, &oldTerm);
    newTerm = oldTerm;

    newTerm.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    newTerm.c_oflag &= ~OPOST;
    newTerm.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    newTerm.c_cflag &= ~(CSIZE | PARENB);
    newTerm.c_cflag |= CS8;
    newTerm.c_cc[VMIN] = 1;
    newTerm.c_cc[VTIME] = 0;

    tcsetattr(0, TCSANOW, &newTerm);
}

void exitRawMode(void)
{
    tcsetattr(0, TCSAFLUSH, &oldTerm);
}

void enableAltBuffer(void)
{
    write(STDOUT_FILENO, "\033[?1049h", 8);
}

void disableAltBuffer(void)
{
    write(STDOUT_FILENO, "\033[?1049l", 8);
}

static void tui_buf_write(const char* s, size_t len)
{
    if (tui_buf_len + len >= TUI_BUF_SIZE) {
        write(STDOUT_FILENO, tui_buf, tui_buf_len);
        tui_buf_len = 0;
    }
    memcpy(tui_buf + tui_buf_len, s, len);
    tui_buf_len += len;
}

void clear(void)
{
    tui_buf_write("\x1b[2J\x1b[H", 7);
}

void move(int row, int col)
{
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", row + 1, col + 1);
    tui_buf_write(buf, len);
}

void printw(const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    tui_buf_write(buf, len);
}

void mvprintw(int row, int col, const char* fmt, ...)
{
    move(row, col);
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    tui_buf_write(buf, len);
}

void refresh(void)
{
    if (tui_buf_len > 0) {
        write(STDOUT_FILENO, tui_buf, tui_buf_len);
        tui_buf_len = 0;
    }
}

void getmaxyx(int* rows, int* cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        *rows = 24;
        *cols = 80;
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    }
}

int readKey(void)
{
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;
    if (c == 0x1b) {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESC;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESC;
        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
            }
        }
        return ESC;
    }
    if (c <= 26) return c;
    if (c == 127) return BACKSPACE;
    if (c == '\r' || c == '\n') return CR;
    return c;
}

void draw(buffer* buf) {
    if (!buf) return;

    clear();

    int max_row = state.winrows;
    int max_col = state.wincols;

    for (int i = 0; i < max_row; i++) {
        int row_index = i + state.rowoff;
        if (row_index >= buf->numrows) break;

        row* r = &buf->rows[row_index];
        char line[max_col + 1];
        int len = (r->length < max_col) ? r->length : max_col;
        strncpy(line, r->line, len);
        line[len] = '\0';

        mvprintw(i, 0, "%s", line);
    }

    if (state.cy >= buf->numrows) state.cy = buf->numrows - 1;
    if (state.cy < 0) state.cy = 0;
    if (state.cx > buf->rows[state.cy].length) state.cx = buf->rows[state.cy].length;
    if (state.cx < 0) state.cx = 0;

    if (state.cy < state.rowoff) state.rowoff = state.cy;
    else if (state.cy >= state.rowoff + state.winrows) state.rowoff = state.cy - state.winrows + 1;

    move(state.cy - state.rowoff, state.cx);
    refresh();
}
