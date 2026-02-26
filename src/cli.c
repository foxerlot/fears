#include <stdio.h>
#include <stdlib.h>
#include "cli.h"

static const char* usage = "Usage: fears [options] [input]";

const char* parse_args(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "%s\n", usage);
        exit(1);
    }
    return argv[1];
}