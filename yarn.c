#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "yarn.h"
#include "scroll.h"

void run(struct notification *n)
{
    char *message;
    char space = ' ';

    printf("sizeof: %lui\n", sizeof(n->summary));
    
    message = append(n->body, &space, 30);
    scroll(message, n->summary, 10, 40, 0);
    free(message);
}
