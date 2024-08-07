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
    buffer->size = 0;
    buffer->data = NULL;

    f = fopen(filename, "r+");
    assert(f != NULL);

    while (fgets(buff, LINE_MAX_LEN, f) != NULL)
    {
        buffer->data = realloc(buffer->data, sizeof(char *) * (buffer->size + 1));
        assert(buffer->data);
        buffer->data[buffer->size] = malloc(sizeof(char *) * (strlen(buff) + 1));
        assert(buffer->data[buffer->size]);

        c = buff;
        while (*c != '\0')
        {
            if (*c == '\n')
                *c = ' ';
            else if (*c == '\0')
                *c = ' ';

            ++c;
        }

        strcpy(buffer->data[buffer->size], buff);
        ++buffer->size;
    }
    fclose(f);
    strncpy(buffer->filename, filename, FNAME_LEN);
}

void
close_buffer(Buffer *buffer)
{
}

void
save_buffer(Buffer *buffer)
{
}
