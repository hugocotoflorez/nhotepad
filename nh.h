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
#include "termstuff.h"
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

#define STATUS_POSI_I 1
#define STATUS_POSI_J MARGIN_LEFT + 1
#define STATUS_PRINT(msg)                                   \
    {                                                       \
        term_cursor_save_current_possition();               \
        term_cursor_hide();                                 \
        term_cursor_position(STATUS_POSI_I, STATUS_POSI_J); \
        term_apply_font_effects(ITALIC);                    \
        term_apply_color(GREEN, BRIGHT_BG);                 \
        term_apply_color(BLACK, BRIGHT_FG);                 \
        printf("%s", msg);                                  \
        term_apply_font_effects(ITALIC_OFF);                \
        term_cursor_restore_saved_position();               \
        term_cursor_show();                                 \
    }

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
    int   length;
    int   capacity;
    char *data;
} line_t;

#define AL_INCREMENT 2;
#define ELEM_ERR (line_t){ 0 };

typedef struct __araylist
{
    line_t **data;
    char     filename[FNAME_LEN];
    int      capacity;
    int      length;
} Buffer;

typedef void (*Action)(void);

#define bind_ctrl(key, action) bind(CTRL_M, key, action);

// input.c
void bind(enum MOD mod, char key, Action action);
void kb_handler();
void nh_arrowkey(char key);
void nh_special(AsciiControlCode key);

// ui.c
void kb_die();              /* Bindable function */
void arrowup();             /* Bindable function */
void arrowdown();           /* Bindable function */
void arrowright();          /* Bindable function */
void arrowleft();           /* Bindable function */
void save_current_buffer(); /* Bindable function */
void backspace();           /* Bindable function */
void enter();               /* Bindable function */
void tab();                 /* Bindable function */
void load_buffer(Buffer buff);
void nh_write(char c);

// file.c
void open_buffer(const char *filename, Buffer *buffer);
void close_buffer(Buffer *buffer);
void save_buffer(Buffer *buffer);


// buffer.c
Buffer  buffer_create(int capacity);
void    buffer_modify(Buffer *buffer, int index, line_t element);
int     buffer_append(Buffer *buffer, line_t element);
void    buffer_insert(Buffer *buffer, int index, line_t element);
line_t  buffer_remove(Buffer *buffer, int index);
line_t *buffer_get(Buffer buffer, int index);
line_t  newline(char *str);
int     line_append(line_t *line, char c);
void    line_modify(line_t *line, int index, char c);
void    line_insert(line_t *line, int index, char c);
char    line_get(line_t line, int index);
char    line_remove(line_t *line, int index);

#endif // !_NH_H
