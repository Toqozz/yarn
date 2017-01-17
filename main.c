#include <libconfig.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "datatypes.h"
#include "dbus.h"
#include "cfg.h"
#include "yarn.h"
#include "parse.h"
#include "stdio.h"
#include "utils.h"

Variables opt;

int
main (int argc, char *argv[])
{
    const char *home = getenv("HOME");
    char config_path[100];

    // TODO, move this to somewhere else?
    sprintf(config_path, "%s/.config/yarn/config", home);
    printf("%s\n", config_path);

    config_t cfg;
    config_init(&cfg);

    // Read config in from file.
    Config *c = cfg_create();
    cfg_read(cfg, config_path, c);

    // Read config into stack (& convert).
    cfg_assign(c);

    // Start yarn.
    yarn_init();

    // We're done, the config can finally be let go.
    config_destroy(&cfg);
    free(c);

    return 0;
}
