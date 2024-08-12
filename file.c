#include "nh.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
open_buffer(const char *filename, Buffer *buffer)
{
    char  line_str[LINE_MAX_LEN];
    FILE *f;
    char *c = NULL;

    *buffer = buffer_create(1);

    f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error while trying to open the file");
        exit(1);
    }

    while (fgets(line_str, LINE_MAX_LEN, f) != NULL)
    {
        c = line_str;
        while (*c != '\0')
        {
            if (*c == '\n')
                *c = '\0';
            else
                ++c;
        }

        buffer_append(buffer, newline(line_str));
    }

    if (c == NULL) // empty file
        buffer_append(buffer, newline("\0"));
    strncpy(buffer->filename, filename, FNAME_LEN);
    fclose(f);
}

void
close_buffer(Buffer *buffer)
{
    while (buffer->length > 0)
        free(buffer_remove(buffer, 0).data);
}

void
save_buffer(Buffer *buffer)
{
    line_t *line;
    FILE   *f   = fopen(buffer->filename, "w");
    int     len = buffer->length;
    assert(f != NULL);

    for (int i = 0; i < len; i++)
    {
        line = buffer_get(*buffer, i);
        fprintf(f, "%s\n", line->data);
    }

    STATUS_PRINT("Saved");
    fclose(f);
}
