#include <libconfig.h>

#include "datatypes.h"
#include "dbus.h"
#include "config.h"
#include "yarn.h"
#include "parse.h"
#include "stdio.h"

Variables opt;

int
main (int argc, char *argv[])
{
    config_t cfg;
    config_init(&cfg);

    // Read config in from file.
    Config *c = cfg_create();
    read_config(cfg, "./config", c);

    // Read config into stack (& convert), destroys "&cfg".
    // false^
    parse_config(c);

    yarn_init();

    config_destroy(&cfg);
}
