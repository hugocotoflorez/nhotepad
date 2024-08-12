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
get_relative_position()
{
    return (Vec2d){
        .i = buffer_posi.i + cursor_posi.i - 1 - MARGIN_TOP,
        .j = cursor_posi.j + buffer_posi.j - 1 - MARGIN_LEFT,
    };
}

void
cursor_left_adjust()
{
    Vec2d   relposi  = get_relative_position();
    line_t *currline = buffer_get(buffer, relposi.i);

    while (cursor_posi.j + buffer_posi.j - 1 - MARGIN_LEFT > currline->length)
    {
        --cursor_posi.j;
        --relposi.j;
        term_cursor_back(1);
    }
    if (currline->data[relposi.j - 1] == '\0' && relposi.j > 0)
    {
        --cursor_posi.j;
        term_cursor_back(1);
    }
}


// }}}

// {{{ arrow movement

void
arrowup()
{
    Vec2d relposi = get_relative_position();

    // cursor can only be here if at first line due to MARGIN_LINES
    if (relposi.i == 0)
        return;

    if (relposi.i - buffer_posi.i <= MARGIN_LINES && buffer_posi.i > 0) // into scroll section
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
    Vec2d relposi = get_relative_position();

    if (relposi.i == buffer.length - 1)
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
    Vec2d relposi         = get_relative_position();
    short horizontal_size = fullscreen.j - MARGIN_LEFT - MARGIN_RIGHT;

    term_cursor_save_current_possition();
    term_cursor_position(cursor_posi.i, MARGIN_LEFT + 1);
    term_apply_color(FG_COLOR, FG);
    term_apply_color(BG_COLOR, BG);

    printf("%-*s", horizontal_size,
           buffer_get(buffer, buffer_posi.i + relposi.i)->data);

    term_apply_font_effects(NORMAL);
    term_cursor_restore_saved_position();
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

    term_cursor_position(2, MARGIN_LEFT + 1);
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
    Vec2d   relposi = get_relative_position();
    Vec2d   curr_cursor_posi;
    line_t *currline = buffer_get(buffer, relposi.i);
    int     nullchar_before_cursor =
    currline->data[relposi.j - 1] == '\0' && relposi.j > 0;
    STTPRT(1, "len %d < %d   ", currline->length, LINE_MAX_LEN);
    if (currline->length < LINE_MAX_LEN)
    {
        if (nullchar_before_cursor)
        {
            --relposi.j;
        }

        line_insert(currline, relposi.j, c);
        putchar(c);
        print_cursorline();
        ++cursor_posi.j;

        if (nullchar_before_cursor)
        {
            --cursor_posi.j;
            term_cursor_back(1);
        }
    }
    else
    {
        if (relposi.j == LINE_MAX_LEN - 1)
        {
            buffer_insert(&buffer, relposi.i + buffer_posi.i + 1, newline("\0"));
            ++cursor_posi.i;
            cursor_posi.j = MARGIN_LEFT + 1;
            term_cursor_position(cursor_posi.i, cursor_posi.j);
            nh_write(c);
        }
        else
        {
            line_insert(currline, relposi.j, c);
            putchar(c);
            ++cursor_posi.j;
            curr_cursor_posi = cursor_posi;
            c                = line_remove(currline, currline->length - 2);
            ++cursor_posi.i;
            cursor_posi.j = MARGIN_LEFT + 1;
            term_cursor_position(cursor_posi.i, cursor_posi.j);
            if (relposi.i + buffer_posi.i == buffer.length - 1)
                buffer_insert(&buffer, relposi.i + buffer_posi.i + 1, newline("\0"));
            nh_write(c);
            cursor_posi = curr_cursor_posi;
            term_cursor_position(cursor_posi.i, cursor_posi.j);
        }

        print_buffer();
    }
}

// }}}

// {{{ Special keys
void
backspace()
{
    Vec2d   curr_cursor_posi;
    Vec2d   relposi  = get_relative_position();
    line_t *currline = buffer_get(buffer, relposi.i);
    char   *temp;

    if (relposi.j > 0)
    {
        line_remove(currline, relposi.j - 1);
        term_cursor_back(1);
        --cursor_posi.j;
    }
    else if (relposi.i > 0)
    {
        --cursor_posi.i;
        cursor_posi.j = LINE_MAX_LEN + MARGIN_LEFT;
        cursor_left_adjust();
        curr_cursor_posi = cursor_posi;

        temp = currline->data + relposi.j;
        while (*temp != '\0')
        {
            nh_write(*temp);
            ++temp;
        }

        buffer_remove(&buffer, relposi.i);

        cursor_posi = curr_cursor_posi;
        term_cursor_position(cursor_posi.i, cursor_posi.j);
    }
    else
        return;

    print_buffer();
}

// }}}
