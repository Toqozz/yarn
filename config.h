#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <libconfig.h>
#include "datatypes.h"

int read_config(config_t cfg, char *file, Config *c);

int parse_config(Config *c);

#endif
