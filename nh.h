#ifndef _NH_H
#define _NH_H

/*
 * NHotepad - text editor
 * Author: Hugo Coto Florez
 *
 * Chech README.md for more info.
 */

/*
 * How pseudo-threading work
 * This editor run on a single thread
 * Actions are called from the keyboard manager
 * The next keypress is read once the previous action finished
 * If no keypress it spins, sleeping SLEEP_MS miliseconds
 */

// skip check for duplicate binds at binding new trigger [bind()]
#define SKIP_DUP_TRIGGER

// ------ COLORS -------
#define BG_COLOR BLACK
#define FG_COLOR WHITE
// ---------------------

#define LINE_MAX_LEN 80
#define FNAME_LEN 30 // filename length

#define MARGIN_LINES 8

#define SLEEP_MS 100

// ui stuff
#define MARGIN_TOP 1 // have to be equal or greater than header height
#define MARGIN_LEFT 1
#define MARGIN_RIGHT 1
#define MARGIN_BOTTOM 1

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
    char   filename[FNAME_LEN];
} Buffer;

typedef void (*Action)(void);

// input.c
void bind(enum MOD mod, char key, Action action);
void kb_handler();

// ui.c
void load_buffer(Buffer buff);
void nh_write(char c);
void kb_die();     /* Bindable function */
void arrowup();    /* Bindable function */
void arrowdown();  /* Bindable function */
void arrowright(); /* Bindable function */
void arrowleft();  /* Bindable function */

// file.c
void open_buffer(const char *filename, Buffer *buffer);
void close_buffer(Buffer *buffer);
void save_buffer(Buffer *buffer);

#endif // !_NH_H
