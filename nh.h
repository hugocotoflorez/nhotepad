#ifndef _NH_H
#define _NH_H

/*
 * NHotepad - text editor
 * Author: Hugo Coto Florez
 *
 * Chech README.md for more info.
 */

// skip check for duplicate binds at binding new trigger [bind()]
#define SKIP_DUP_TRIGGER

// ------ COLORS -------
#define BG_COLOR BLACK
#define FG_COLOR WHITE
// ---------------------

#define LINE_MAX_LEN 80

typedef struct
{
    short i, j;
} Vec2d;


enum MOD
{
    NONE_M,
    CTRL_M,
};

typedef struct
{
    char **data;
    int    size; // lines
} Buffer;

typedef void (*Action)(void);

// input.c
void bind(enum MOD mod, char key, Action action);
void kb_handler();

// ui.c
void load_buffer(Buffer buff);
void nh_write(char c);
void print_buffer();

// file.c
void open_buffer(const char *filename, Buffer *buffer);
void close_buffer(Buffer *buffer);
void save_buffer(Buffer *buffer);

#endif // !_NH_H
