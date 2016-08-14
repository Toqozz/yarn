#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <cairo-xlib.h>

#include "yarn.h"
#include "datatypes.h"
#include "parse.h"
#include "draw.h"

// Option initialization.
double timeout = 10;
int  margin = 5, upper = 0,
     xpos = 0, ypos = 0,
     width = 0, height = 0,
     gap = 7, rounding = 2;
char *font = "Incosolata 14";
char *dimensions = "300x30+300+300";

//TODO check if thread is running or not first please!!!

//void run(struct notification *n)
void
*run(void *arg)
{
    struct notification *n = (struct notification*) arg;

    parse(dimensions, &xpos, &ypos, &width, &height);

    printf("sizeof: %lu\n", sizeof(n));

    printf("%s\n", n->summary);

    printf("running now!\n");

    draw(n->summary);

    //pthread_mutex_destroy(&stack_mutex);
    return NULL;
}
