#include "nh.h"
#include <stdlib.h>
#include <unistd.h>


int
main()
{
    Buffer buffer;
    open_buffer("main.c", &buffer);
    load_buffer(buffer);

    bind(CTRL_M, 'q', kb_die);
    bind(CTRL_M, 'h', arrowleft);
    bind(CTRL_M, 'j', arrowdown);
    bind(CTRL_M, 'l', arrowright);
    bind(CTRL_M, 'k', arrowup);

    kb_handler();

    return 0;
}
