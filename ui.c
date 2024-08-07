#include "nh.h"
#include "termstuff.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

// globals
Vec2d  cursor_posi;            // screen cursor absolute position
Vec2d  buffer_posi = { 0, 0 }; // buffer's pointer position
Buffer buffer      = { 0 };    // file text buffer
Vec2d  fullscreen  = { 0, 0 };

void print_cursorline();
void print_buffer();

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

__attribute__((constructor)) void
init_ui()
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

// {{{ arrow movement

void
arrowup()
{
    if (cursor_posi.i > MARGIN_LINES + MARGIN_TOP + 1)
    {
        --cursor_posi.i;
        term_cursor_up(1);
        return;
    }
    if (buffer_posi.i > 0)
    {
        --buffer_posi.i;
        print_buffer();
        return;
    }
    if (cursor_posi.i > MARGIN_TOP + 1)
    {
        --cursor_posi.i;
        term_cursor_up(1);
        return;
    }
}

void
arrowdown()
{
    if (cursor_posi.i < fullscreen.i - MARGIN_LINES - MARGIN_BOTTOM)
    {
        ++cursor_posi.i;
        term_cursor_down(1);
        return;
    }
    if (buffer_posi.i - 1 <= buffer.size - fullscreen.i)
    {
        ++buffer_posi.i;
        print_buffer();
        return;
    }
    if (cursor_posi.i < fullscreen.i - MARGIN_BOTTOM)
    {
        ++cursor_posi.i;
        term_cursor_down(1);
        return;
    }
}

void
arrowright()
{
    if (cursor_posi.j < fullscreen.j - MARGIN_RIGHT)
    {
        ++cursor_posi.j;
        term_cursor_forward(1);
    }
}
void
arrowleft()
{
    if (cursor_posi.j > MARGIN_LEFT + 1)
    {
        --cursor_posi.j;
        term_cursor_back(1);
    }
}
/// }}}

// {{{ Print stuff functions

void
print_cursorline()
{
    print_buffer();
}

void
print_header()
{
    short horizontal_size = fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT;

    term_cursor_position(1, MARGIN_LEFT + 1);
    printf("%-*s", horizontal_size, "");
    term_cursor_position(2, MARGIN_LEFT + 1);
    printf("%-*s", horizontal_size, "");

    term_cursor_position(1, MARGIN_LEFT + 1);
    term_apply_color(BLUE, FG);
    term_apply_font_effects(BOLD);
    term_apply_font_effects(UNDERLINE);
    printf("nhotepad");
    term_apply_font_effects(UNDERLINE_OFF);
    printf("  (file: ");
    term_apply_font_effects(ITALIC);
    printf("%s", buffer.filename);
    term_apply_font_effects(ITALIC_OFF);
    printf(")  ");
    term_apply_color(BLACK, BRIGHT_FG);
    term_apply_font_effects(INVERSE);
    printf("[C-q] Quit | [C-s] Save | [C-o] Open");
    term_apply_font_effects(INVERSE_OFF);

    term_apply_font_effects(NORMAL);

    term_cursor_position(cursor_posi.i, cursor_posi.j);
}

void
print_buffer()
{
    short i, range, horizontal_size;
    Vec2d curr_cursor_posi = cursor_posi;

    term_cursor_save_current_possition();

    term_clear_screen();
    print_header(); // cursor is placed just after header

    term_apply_color(FG_COLOR, FG);
    term_apply_color(BG_COLOR, BG);

    horizontal_size = fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT;
    cursor_posi.i   = MARGIN_TOP;

    if (buffer.size < fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM)
        range = buffer.size;
    else
        range = fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM;


    for (i = 0; i < range; i++)
    {
        term_cursor_position(++cursor_posi.i, MARGIN_LEFT + 1);
        printf("%-*s", horizontal_size, buffer.data[buffer_posi.i + i]);
    }
    term_apply_font_effects(NORMAL);

    term_cursor_restore_saved_position();
    cursor_posi = curr_cursor_posi;
}

// }}}

void
nh_write(char c)
{
    // TODO
    assert(buffer.data != NULL);
}
