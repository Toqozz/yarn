#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "yarn.h"

//static struct notification *n;

void run(struct notification *n)
{
    printf("sizeof: %lui\n", sizeof(n->summary));
    printf("summary from yarn.c%s\n", n->summary);
}
