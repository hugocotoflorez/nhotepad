#include "nh.h"
#include <stdlib.h>
#include <string.h>

line_t
newline(char *str)
{
    line_t newl;
    newl.size = strlen(str);
    newl.data = strdup(str);
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
    if (index >= buffer->length)
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
    temp = buffer_get(*buffer, index);
    free(buffer->data[index]);
    for (int i = index; i < buffer->length; i++)
    {
        buffer->data[i] = buffer->data[i + 1];
    }
    --buffer->length;
    return temp;
}


line_t
buffer_get(Buffer buffer, int index)
{
    if (index >= buffer.length)
        return ELEM_ERR;
    return *((line_t *) buffer.data[index]);
}
