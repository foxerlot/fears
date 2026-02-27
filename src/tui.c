#include <termios.h>

struct termios oldTerm, newTerm;

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
