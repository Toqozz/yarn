#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <libconfig.h>
#include "datatypes.h"

Config
*cfg_create(void);

int cfg_read(config_t cfg, char *file, Config *c);

int cfg_assign(Config *c);

#endif
