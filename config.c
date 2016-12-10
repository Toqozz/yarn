#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>

#include "config.h"
#include "datatypes.h"
#include "parse.h"

int parse_config(char *file, Config *c)
{
    config_t cfg;
    config_setting_t *setting;

    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, file))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);

        return(EXIT_FAILURE);
    }

    /* Get the max notifications. */
    if (!config_lookup_int(&cfg, "max_notifications", &c->max_notifications))
        fprintf(stderr, "No 'max_notifications' setting in configuration file.\n");

    if (!config_lookup_int(&cfg, "gap", &c->gap))
        fprintf(stderr, "No 'gap' setting in configuration file.\n");

    setting = config_lookup(&cfg, "notification");
    if (setting != NULL)
    {
        config_setting_lookup_string(setting, "geometry", &c->geometry);
        config_setting_lookup_string(setting, "summary_color", &c->summary_color);
        config_setting_lookup_string(setting, "body_color", &c->body_color);
        config_setting_lookup_string(setting, "bgcolor", &c->bgcolor);
        config_setting_lookup_string(setting, "fgcolor", &c->fgcolor);
        config_setting_lookup_string(setting, "bdcolor", &c->bdcolor);

        config_setting_lookup_float(setting, "interval", &c->interval);
        config_setting_lookup_int(setting, "timeout", &c->timeout);
        config_setting_lookup_int(setting, "scroll_speed", &c->scroll_speed);

        config_setting_lookup_string(setting, "font", &c->font);

        config_setting_lookup_int(setting, "margin", &c->margin);
        config_setting_lookup_int(setting, "overline", &c->overline);
        config_setting_lookup_int(setting, "bw", &c->bw);
        config_setting_lookup_int(setting, "rounding", &c->rounding);

        printf("max_notif: \t%d\ndimensions: \t%s\nsummary_color: \t%s\nbody_color: \t%s\nbgcolor: \t%s\nfgcolor: \t%s\nbdcolor \t%s\n", c->max_notifications, c->geometry, c->summary_color, c->body_color, c->bgcolor, c->fgcolor, c->bdcolor);
    }

    config_destroy(&cfg);

    return(EXIT_SUCCESS);
}

int parse_convert(Config *c, Variables *v)
{
    //char *bc = "ebdbb2";

    parse(c->geometry, &v->xpos, &v->ypos, &v->width, &v->height);

    v->font = c->font;
    v->margin = c->margin;
    v->max = c->max_notifications;
    v->gap = c->gap;
    v->overline = c->overline;
    v->bw = c->bw;
    v->rounding = c->rounding;
}









    /* Output a list of all books in the inventory. */
    //setting = config_lookup(&cfg, "inventory.books");
    //if(setting != NULL)
    //{
        //int count = config_setting_length(setting);
        //int i;
//
        //printf("%-30s  %-30s   %-6s  %s\n", "TITLE", "AUTHOR", "PRICE", "QTY");
//
        //for(i = 0; i < count; ++i)
        //{
            //config_setting_t *book = config_setting_get_elem(setting, i);
//
            ///* Only output the record if all of the expected fields are present. */
            //const char *title, *author;
            //double price;
            //int qty;
//
            //if(!(config_setting_lookup_string(book, "title", &title)
                //&& config_setting_lookup_string(book, "author", &author)
                //&& config_setting_lookup_float(book, "price", &price)
                //&& config_setting_lookup_int(book, "qty", &qty)))
                //continue;
//
            //printf("%-30s  %-30s  $%6.2f  %3d\n", title, author, price, qty);
        //}
        //putchar('\n');
    //}
//
    ///* Output a list of all movies in the inventory. */
    //setting = config_lookup(&cfg, "inventory.movies");
    //if(setting != NULL)
    //{
        //unsigned int count = config_setting_length(setting);
        //unsigned int i;
//
        //printf("%-30s  %-10s   %-6s  %s\n", "TITLE", "MEDIA", "PRICE", "QTY");
        //for(i = 0; i < count; ++i)
        //{
            //config_setting_t *movie = config_setting_get_elem(setting, i);
//
            ///* Only output the record if all of the expected fields are present. */
            //const char *title, *media;
            //double price;
            //int qty;
//
            //if(!(config_setting_lookup_string(movie, "title", &title)
                //&& config_setting_lookup_string(movie, "media", &media)
                //&& config_setting_lookup_float(movie, "price", &price)
                //&& config_setting_lookup_int(movie, "qty", &qty)))
                //continue;
//
            //printf("%-30s  %-10s  $%6.2f  %3d\n", title, media, price, qty);
        //}
        //putchar('\n');
    //}
//
