#include "nh.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int
main(int argc, char *argv[])
{
    Buffer buffer;

    if (argc == 2)
    {
        open_buffer(argv[1], &buffer);
    }

    else
    {
        open_buffer("example.c", &buffer);
    }

    load_buffer(buffer);

    bind_ctrl('q', kb_die);
    bind_ctrl('h', arrowleft);
    bind_ctrl('j', arrowdown);
    bind_ctrl('l', arrowright);
    bind_ctrl('k', arrowup);
    bind_ctrl('s', save_current_buffer);

    kb_handler();

    return 0;
}
