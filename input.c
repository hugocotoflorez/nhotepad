#include "nh.h"
#include "termstuff.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/*
 * About:  Keyboard handler
 * Author: Hugo Coto Florez
 *
 * -- Queues
 * Tested parts: enqueue, dequeue, search.
 * If other functions are needed, please test it (message for myself)
 */

// type defs {{{

/*
 * A funcion 'action' would be called after mod+key is pressed.
 */
typedef struct
{
    enum MOD mod;    // key modifier
    char     key;    // key
    Action   action; // function defined in nh.h
} Trigger;

// queue node
typedef struct __node_t
{
    struct __node_t *next;    // next queue node or null
    Trigger          trigger; // mod + key and action
} node_t;

/*
 * Queue base type.
 */
typedef struct __queue
{
    node_t *head;
} Queue;

//}}}

// trigger-queue functions {{{

// actions triggered by a key and a modifier
Queue         triggers_queue;
unsigned char is_active_q = 0;

__attribute__((constructor)) void
queue_init()
{
    // create a dummy node as the last node.
    triggers_queue.head = calloc(1, sizeof(node_t));
    assert(triggers_queue.head != NULL);
    is_active_q = 1;
}

void
enqueue(Trigger new_trigger)
{
    node_t *temp;
    node_t *new;
    assert(is_active_q == 1);

    // create new node
    new = malloc(sizeof(node_t));
    assert(new != NULL);
    *new = (node_t){
        .trigger = new_trigger,
        .next    = NULL,
    };

    // goto last node
    temp = triggers_queue.head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = new;
}

void
dequeue()
{
    node_t *temp;
    assert(is_active_q == 1);

    if (triggers_queue.head->next != NULL)
    {
        temp                      = triggers_queue.head->next;
        triggers_queue.head->next = triggers_queue.head->next->next;
    }
    free(temp);
}

void
remove_node(node_t *node)
{
    node_t *temp = node->next;
    assert(is_active_q == 1);
    node->next = node->next->next;
    free(temp);
}

Action
search(enum MOD mod, char key)
{
    node_t *temp;
    assert(is_active_q == 1);
    temp = triggers_queue.head;
    while (temp->next != NULL)
    {
        if (temp->next->trigger.key == key)
            if (temp->next->trigger.mod == mod)
                return temp->next->trigger.action;

        temp = temp->next;
    }
    return NULL;
}

node_t *
search_node(enum MOD mod, char key)
{
    node_t *temp;
    assert(is_active_q == 1);

    temp = triggers_queue.head;
    while (temp->next != NULL)
    {
        if (temp->next->trigger.key == key)
            if (temp->next->trigger.mod == mod)
                return temp;

        temp = temp->next;
    }

    return NULL;
}

// }}}

void
bind(enum MOD mod, char key, Action action)
{
    assert(mod == NONE_M || mod == CTRL_M); // invalid mod

#ifndef SKIP_DUP_TRIGGER
    node_t *temp = search_node(mod, key);
    // if mod+key is already binded
    if (temp != NULL)
        remove_node(temp);
#endif

    enqueue((Trigger){
    .mod    = mod,
    .key    = key,
    .action = action,
    });
}

// {{{ Raw mode

struct termios origin_termios;

void
disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin_termios);
}

void
enableRawMode()
{
    tcgetattr(STDIN_FILENO, &origin_termios);
    struct termios raw;
    raw.c_iflag &=
    ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    raw.c_cflag &= ~(CSIZE | PARENB);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// }}}

void
mssleep(int milliseconds)
{
    struct timespec t;
    struct timespec _t;
    t.tv_sec  = milliseconds / 1000;
    t.tv_nsec = (milliseconds % 1000) * 1e6;
    nanosleep(&t, &_t);
}

unsigned char KB_ACTIVE = 1;

void
kb_die()
{
    KB_ACTIVE = 0;
    term_clear_screen();
}

void
kb_handler()
{
    ssize_t ret;
    Action  action;
    char    key;

    enableRawMode();

    while (KB_ACTIVE)
    {
        ret = read(STDIN_FILENO, &key, 1);
        if (ret != 0)
        {
            // check if ctrl mod
            if (key > 0x0 && key < 0x1b)
            {
                if ((action = search(CTRL_M, key + 0x60)) != NULL)
                    action();
            }
            else if (key == '\033') // the following code may not work for everyone, said stackoverflow.
            {
                if (read(STDIN_FILENO, &key, 1)) // [ after \033
                    if (read(STDIN_FILENO, &key, 1))
                    {
                        switch (key)
                        {
                            case 'A':
                                arrowup();
                                break;
                            case 'B':
                                arrowdown();
                                break;
                            case 'C':
                                arrowright();
                                break;
                            case 'D':
                                arrowleft();
                                break;
                            default:
                                nh_write(key);
                                break;
                        }
                    }
            }
            else
                nh_write(key);
        }
        else
            mssleep(SLEEP_MS);

        fflush(stdout);
    }

    disableRawMode();
}
