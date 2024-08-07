#include "nh.h"
#include <stdlib.h>
#include <unistd.h>


int
main()
{
    Buffer buffer;
    open_buffer("nh.h", &buffer);
    load_buffer(buffer);


    print_buffer();
    kb_handler();

    return 0;
}
