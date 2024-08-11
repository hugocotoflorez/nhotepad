#include "nh.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
open_buffer(const char *filename, Buffer *buffer)
{
    char  buff[LINE_MAX_LEN];
    FILE *f;
    char *c;

    *buffer = buffer_create(1);

    f = fopen(filename, "r+");
    if (f == NULL)
    {
        fprintf(stderr, "Error while trying to open the file");
        exit(1);
    }

    while (fgets(buff, LINE_MAX_LEN, f) != NULL)
    {
        c = buff;
        while (*c != '\0')
        {
            if (*c == '\n')
                *c = '\0';
            else
                ++c;
        }

        buffer_append(buffer, newline(buff));
    }
    strncpy(buffer->filename, filename, FNAME_LEN);
    fclose(f);
}

void
close_buffer(Buffer *buffer)
{
}

void
save_buffer(Buffer *buffer)
{
}
