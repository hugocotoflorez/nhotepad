#include "nh.h"
#include "termstuff.h"
#include <assert.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/ioctl.h>

// globals
Vec2d  cursor_posi = { 1, 1 }; // screen cursor absolute position
Vec2d  buffer_posi = { 0, 0 }; // buffer pointer position
Buffer buffer      = { 0 };    // file text buffer
Vec2d  fullscreen  = { 0, 0 };

#define MARGIN_TOP 2
#define MARGIN_LEFT 6
#define MARGIN_RIGHT 6
#define MARGIN_BOTTOM 2

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
    fullscreen = get_fullscreen();
}

Vec2d
relative_cursor_posi(Vec2d cp)
{
    return (Vec2d){
        .i = cursor_posi.i - (1 + MARGIN_TOP),
        .j = cursor_posi.j - (1 + MARGIN_LEFT),
    };
}

/*
void
printline(short line)
{
    term_apply_color(FG_COLOR, FG);
    term_apply_color(BG_COLOR, BG);
}
 */

void
load_buffer(Buffer buff)
{
    buffer = buff;
}


void
print_header()
{
    printf("%-*s", fullscreen.j, "    nhotepad    [Filename]");
    printf("%-*s", fullscreen.j, "    Options | Options");
}

void
print_buffer()
{
    short i, range, horizontal_size;

    term_clear_screen();
    term_apply_color(FG_COLOR, FG);
    term_apply_color(BG_COLOR, BG);

    print_header();

    horizontal_size = fullscreen.j - MARGIN_LEFT;
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
}


void
nh_write(char c)
{
    assert(buffer.data != NULL);
}
