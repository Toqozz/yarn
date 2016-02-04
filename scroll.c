#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include "scroll.h"

#define BUFSIZE 2048

// Append a character to a string a set amount of times.
char *append(char *dest, int times)
{
    int i = 0;
    char add = ' ';
    // calloc(extra space + original length + extra space + 0/)
    char *concat = calloc((times + strlen(dest) + times + 1), 1);
    assert(concat != NULL);

    {
        for(i = 0; i < times; i++)
            strncat(concat, &add, 1);
        strncat(concat, dest, strlen(concat) + strlen(dest));
    }
    return concat;
}

void scroll(char *string, char *beginning, int interval, int characters, int carriage)
{
    int i = 0;
    // Nanosleep requires a timespec struct.
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = interval*10000000;

    /* For each character of a string, print a range of characters
       -- works better than you might think. */
    for(i = 0; string[i] != '\0'; i++)
    {
        nanosleep(&req, &req);

        // Sometimes we want to use a carriage return.
        if(carriage == 0) printf("%s%.*s\n", beginning, characters, &string[i]);
        else printf("%s%.*s \b\r", beginning, characters, &string[i]);

    }
}

    /*
    char string[BUFSIZE];
    char *beginning = "";
    char *final;
    char space = '-';
    int  interval = 20;
    int  characters = 20;
    int  carriage = 0;

    // Create a string with spaces added to the front.
    final = append(string, &space, characters); 

    scoll(final, beginning, interval, characters, carriage);
    free(final);

    return(0);
    */
