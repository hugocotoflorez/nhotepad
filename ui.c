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

// function defs
void  print_cursorline();
void  print_buffer();
void  nh_write(char c);
void  print_header();
void  print_cursorline();
void  arrowleft();
void  arrowright();
void  arrowdown();
void  arrowup();
void  load_buffer(Buffer buff);
Vec2d get_fullscreen();
void  backspace();
void  cursor_left_adjust();
void  swap_lines(line_t *line1, line_t *line2);

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

Vec2d
get_relative_position()
{
    return (Vec2d){
        .i = buffer_posi.i + cursor_posi.i - 1 - MARGIN_TOP,
        .j = cursor_posi.j + buffer_posi.j - 1 - MARGIN_LEFT,
    };
}

// }}}

// {{{ arrow movement

void
arrowup()
{
}

void
arrowdown()
{
}

void
arrowright()
{
}
void
arrowleft()
{
}
/// }}}

// {{{ Print stuff functions

void
print_cursorline()
{
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

    if (buffer.length < fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM)
        range = buffer.length;
    else
        range = fullscreen.i - MARGIN_TOP - MARGIN_BOTTOM;


    for (i = 0; i < range; i++)
    {
        term_cursor_position(++cursor_posi.i, MARGIN_LEFT + 1);
        printf("%-*s", horizontal_size,
               buffer_get(buffer, buffer_posi.i + i).data);
    }
    term_apply_font_effects(NORMAL);

    term_cursor_restore_saved_position();
    cursor_posi = curr_cursor_posi;
}

void
cursor_left_adjust()
{
    Vec2d relposi = get_relative_position();
    int   len     = buffer_get(buffer, relposi.i + buffer_posi.i).size;
    while (cursor_posi.j + buffer_posi.j - 1 - MARGIN_LEFT > len)
    {
        --cursor_posi.j;
        term_cursor_back(1);
    }
}

void
nh_write(char c)
{
}

// }}}

// {{{ Special keys
void
backspace()
{
}

// }}}
