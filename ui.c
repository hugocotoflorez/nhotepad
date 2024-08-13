#include "nh.h"
#include "termstuff.h"
#include <assert.h>
#include <stdio.h>
#include <sys/ioctl.h>

// static print: print temporal message without moving the cursor
#define STTPRT(row, ...)                      \
    {                                         \
        term_cursor_save_current_possition(); \
        term_cursor_position(3 + row, 50);    \
        printf(__VA_ARGS__);                  \
        term_cursor_restore_saved_position(); \
    }

// globals
Vec2d  cursor_posi;            // screen cursor absolute position
Vec2d  buffer_posi = { 0, 0 }; // buffer's pointer position
Buffer buffer      = { 0 };    // file text buffer
Vec2d  fullscreen  = { 0, 0 };

void print_cursorline();
void print_buffer();

// {{{ misc

Vec2d
get_fullscreen()
{
    Vec2d          size;
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    size.i = w.ws_row;
    size.j = w.ws_col;
    return size;
}

__attribute__((constructor)) static void
init()
{
    fullscreen    = get_fullscreen();
    cursor_posi.i = MARGIN_TOP + 1;
    cursor_posi.j = MARGIN_LEFT + 1;
    term_cursor_position(cursor_posi.i, cursor_posi.j);
}

void
load_buffer(Buffer buff)
{
    buffer = buff;
    print_buffer();
}

void
save_current_buffer()
{
    save_buffer(&buffer);
}

Vec2d
get_buffer_index()
{
    return (Vec2d){
        .i = buffer_posi.i + cursor_posi.i - 1 - MARGIN_TOP,
        .j = cursor_posi.j + buffer_posi.j - 1 - MARGIN_LEFT,
    };
}

void
nullchar_before_cursor()
{
    Vec2d   buffer_index = get_buffer_index();
    line_t *currline     = buffer_get(buffer, buffer_index.i);

    while (buffer_index.j > 0 && currline->data[buffer_index.j - 1] == '\0')
    {
        --cursor_posi.j;
        --buffer_index.j;
        term_cursor_back(1);
    }
}

void
cursor_left_adjust()
{
    Vec2d   buffer_index = get_buffer_index();
    line_t *currline     = buffer_get(buffer, buffer_index.i);

    while (cursor_posi.j + buffer_posi.j - 1 - MARGIN_LEFT > currline->length)
    {
        --cursor_posi.j;
        --buffer_index.j;
        term_cursor_back(1);
    }
    nullchar_before_cursor();
}


// }}}

// {{{ arrow movement

void
arrowup()
{
    Vec2d buffer_index = get_buffer_index();

    // cursor can only be here if at first line due to MARGIN_LINES
    if (buffer_index.i == 0)
        return;

    if (buffer_index.i - buffer_posi.i <= MARGIN_LINES && buffer_posi.i > 0) // into scroll section
    {
        --buffer_posi.i;
        print_buffer();
    }
    else // can move cursor and not in scroll section
    {
        --cursor_posi.i;
        term_cursor_up(1);
    }
    cursor_left_adjust();
}

void
arrowdown()
{
    Vec2d buffer_index = get_buffer_index();

    if (buffer_index.i == buffer.length - 1)
        return;

    if (cursor_posi.i >= fullscreen.i - MARGIN_TOP - MARGIN_LINES &&
        buffer_posi.i + fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM < buffer.length)
    {
        ++buffer_posi.i;
        print_buffer();
    }
    else
    {
        ++cursor_posi.i;
        term_cursor_down(1);
    }
    cursor_left_adjust();
}


void
arrowright()
{
    // without buffer horizontal scroll
    if (cursor_posi.j - 1 < fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT)
    {
        ++cursor_posi.j;
        term_cursor_forward(1);
    }
    cursor_left_adjust();
}

void
arrowleft()
{
    // without buffer horizontal scroll
    if (cursor_posi.j - 1 > MARGIN_LEFT)
    {
        --cursor_posi.j;
        term_cursor_back(1);
    }
    cursor_left_adjust();
}

/// }}}

// {{{ Print stuff functions

void
print_cursorline()
{
    Vec2d buffer_index    = get_buffer_index();
    short horizontal_size = fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT;

    term_cursor_save_current_possition();
    term_cursor_position(cursor_posi.i, MARGIN_LEFT + 1);
    term_apply_color(FG_COLOR, FG);
    term_apply_color(BG_COLOR, BG);

    printf("%-*s", horizontal_size, buffer_get(buffer, buffer_index.i)->data);

    term_apply_font_effects(NORMAL);
    term_cursor_restore_saved_position();
}

void
print_header()
{
    short horizontal_size = fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT;

    term_cursor_position(1, MARGIN_LEFT + 1);
    printf("%-*s", horizontal_size, "");
    /*
    term_cursor_position(2, MARGIN_LEFT + 1);
    printf("%-*s", horizontal_size, "");
     */

    term_cursor_position(1, MARGIN_LEFT + 1);
    term_apply_color(BLUE, FG);
    term_apply_font_effects(BOLD);
    printf("nhotepad");
    term_apply_font_effects(NORMAL);
    term_apply_color(BLUE, FG);
    printf("  (file: ");
    term_apply_font_effects(ITALIC);
    printf("%s", buffer.filename);
    term_apply_font_effects(ITALIC_OFF);
    printf(")  ");

    // term_cursor_position(1, MARGIN_LEFT + 1);
    printf("| ");
    term_apply_font_effects(UNDERLINE);
    printf("Q");
    term_apply_font_effects(UNDERLINE_OFF);
    printf("uit | ");
    term_apply_font_effects(UNDERLINE);
    printf("S");
    term_apply_font_effects(UNDERLINE_OFF);
    printf("ave | ");
    term_apply_font_effects(UNDERLINE);
    printf("O");
    term_apply_font_effects(UNDERLINE_OFF);
    printf("pen |");

    term_apply_font_effects(NORMAL);

    term_cursor_position(cursor_posi.i, cursor_posi.j);
}

void
print_buffer()
{
    short i, range, horizontal_size;
    Vec2d curr_cursor_posi = cursor_posi;

    term_cursor_save_current_possition();
    term_cursor_hide();

    term_clear_screen();
    print_header(); // cursor is placed just after header

    term_apply_color(FG_COLOR, FG);
    term_apply_color(BG_COLOR, BG);

    horizontal_size = fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT;
    cursor_posi.i   = MARGIN_TOP;

    if (buffer.length < fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM)
        range = buffer.length;
    else
        range = fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM;


    for (i = 0; i < range; i++)
    {
        term_cursor_position(++cursor_posi.i, MARGIN_LEFT + 1);
        printf("%-*s", horizontal_size,
               buffer_get(buffer, buffer_posi.i + i)->data);
    }
    term_apply_font_effects(NORMAL);

    term_cursor_show();
    term_cursor_restore_saved_position();
    cursor_posi = curr_cursor_posi;
}

void
nh_write(char c)
{
    Vec2d   buffer_index = get_buffer_index();
    Vec2d   curr_cursor_posi;
    line_t *currline = buffer_get(buffer, buffer_index.i);

    if (currline->length < LINE_MAX_LEN)
    {
        nullchar_before_cursor();

        line_insert(currline, buffer_index.j, c);
        term_cursor_forward(1);
        ++cursor_posi.j;

        nullchar_before_cursor();
    }
    else
    {
        if (buffer_index.j == LINE_MAX_LEN - 1)
        {
            buffer_insert(&buffer, buffer_index.i + 1, newline("\0"));
            arrowdown();
            cursor_posi.j = MARGIN_LEFT + 1;
            term_cursor_position(cursor_posi.i, cursor_posi.j);
            nh_write(c);
        }
        else
        {
            line_insert(currline, buffer_index.j, c);

            term_cursor_forward(1);
            ++cursor_posi.j;
            curr_cursor_posi = cursor_posi;

            c = line_remove(currline, currline->length - 2);

            arrowdown();
            cursor_posi.j = MARGIN_LEFT + 1;
            term_cursor_position(cursor_posi.i, cursor_posi.j);

            if (buffer_index.i == buffer.length - 1) // last line
                buffer_append(&buffer, newline("\0"));

            nh_write(c);
            cursor_posi = curr_cursor_posi;
            term_cursor_position(cursor_posi.i, cursor_posi.j);
        }
    }
    print_buffer();
}

// }}}

// {{{ Special keys
void
backspace()
{ // {{{
    Vec2d   curr_cursor_posi;
    Vec2d   buffer_index = get_buffer_index();
    line_t *currline     = buffer_get(buffer, buffer_index.i);
    char   *temp;

    if (buffer_index.j > 0)
    {
        line_remove(currline, buffer_index.j - 1);
        term_cursor_back(1);
        --cursor_posi.j;
    }
    else if (buffer_index.i > 0)
    {
        --cursor_posi.i;
        cursor_posi.j = LINE_MAX_LEN + MARGIN_LEFT;
        cursor_left_adjust();
        curr_cursor_posi = cursor_posi;

        temp = currline->data + buffer_index.j;
        while (*temp != '\0')
        {
            nh_write(*temp);
            ++temp;
        }

        buffer_remove(&buffer, buffer_index.i);

        cursor_posi = curr_cursor_posi;
        term_cursor_position(cursor_posi.i, cursor_posi.j);
    }
    else
        return;

    print_buffer();
} // }}}

void
enter()
{
    Vec2d   buffer_index = get_buffer_index();
    line_t *currline     = buffer_get(buffer, buffer_index.i);


    buffer_insert(&buffer, buffer_index.i + 1,
                  newline(currline->data + buffer_index.j));

    if (cursor_posi.i == fullscreen.i - MARGIN_BOTTOM)
        ++buffer_posi.i;
    else
        ++cursor_posi.i;

    currline->data[buffer_index.j] = '\0';

    cursor_posi.j = MARGIN_LEFT + 1;
    term_cursor_position(cursor_posi.i, cursor_posi.j);
    print_buffer();
}

void
tab()
{
    Vec2d   buffer_index = get_buffer_index();
    line_t *currline     = buffer_get(buffer, buffer_index.i);
    short   size =
    (LINE_MAX_LEN - currline->length > 4) ? 4 : LINE_MAX_LEN - currline->length;

    for (int i = 0; i < size; i++)
        nh_write(' ');
}

// }}}
