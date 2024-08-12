#include "nh.h"
#include <stdlib.h>
#include <string.h>

line_t
newline(char *str)
{
    line_t newl;
    newl.length   = strlen(str) + 1;
    newl.capacity = strlen(str) + 1;
    newl.data     = strdup(str);
    return newl;
}

Buffer
buffer_create(int capacity)
{
    return (Buffer){
        .capacity = capacity > 0 ? capacity : 1,
        .length   = 0,
        .data     = malloc(sizeof(line_t *) * capacity),
    };
}

void
_buffer_resize(Buffer *buffer)
{
    buffer->capacity *= AL_INCREMENT;
    buffer->data = realloc(buffer->data, buffer->capacity * sizeof(line_t *));
}

int
buffer_append(Buffer *buffer, line_t element)
{
    if (buffer->length == buffer->capacity)
        _buffer_resize(buffer);

    buffer->data[buffer->length]    = malloc(sizeof(line_t));
    *(buffer->data[buffer->length]) = element;

    return buffer->length++;
}

void
buffer_modify(Buffer *buffer, int index, line_t element)
{
    if (index >= buffer->length)
        return;

    *(buffer->data[index]) = element;
}

// i and j MUST be 0<=i,j<length
void
_swap(Buffer *buffer, int i, int j)
{
    line_t *temp    = buffer->data[i];
    buffer->data[i] = buffer->data[j];
    buffer->data[j] = temp;
}

void
buffer_insert(Buffer *buffer, int index, line_t element)
{
    if (index > buffer->length)
        return;

    buffer_append(buffer, element);

    for (int i = buffer->length - 1; i > index; i--)
        _swap(buffer, i, i - 1);
}


line_t
buffer_remove(Buffer *buffer, int index)
{
    line_t temp;

    if (index >= buffer->length)
        return ELEM_ERR;

    temp = *buffer_get(*buffer, index);
    free(buffer->data[index]);

    for (int i = index; i < buffer->length; i++)
        buffer->data[i] = buffer->data[i + 1];

    --buffer->length;

    return temp;
}


line_t *
buffer_get(Buffer buffer, int index)
{
    if (index >= buffer.length)
        return NULL;

    return buffer.data[index];
}

/*
 * LINE STUFF
 */

void
_line_resize(line_t *line)
{
    line->capacity *= AL_INCREMENT;
    line->data = realloc(line->data, line->capacity * sizeof(line_t *));
}

int
line_append(line_t *line, char c)
{
    if (line->length == line->capacity)
        _line_resize(line);

    line->data[line->length] = c;

    return line->length++;
}

void
line_modify(line_t *line, int index, char c)
{
    if (index >= line->length)
        return;

    line->data[index] = c;
}

// i and j MUST be 0<=i,j<length
void
_swapchars(line_t *line, int i, int j)
{
    char temp     = line->data[i];
    line->data[i] = line->data[j];
    line->data[j] = temp;
}

void
line_insert(line_t *line, int index, char c)
{
    if (index > line->length)
        return;

    line_append(line, c);

    for (int i = line->length - 1; i > index; i--)
        _swapchars(line, i, i - 1);
}

char
line_get(line_t line, int index)
{
    if (index >= line.length)
        return '\0';

    return line.data[index];
}

char
line_remove(line_t *line, int index)
{
    char temp;

    if (index >= line->length)
        return '\0';

    temp = line_get(*line, index);

    for (int i = index; i < line->length; i++)
        line->data[i] = line->data[i + 1];

    --line->length;

    return temp;
}
