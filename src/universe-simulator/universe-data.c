#include "universe-data.h"
#include <libconfig.h>

int read_config_file(config_t *cfg, UniverseConfig *universe_config) {
    if (config_lookup_int(cfg, "universe.size", &universe_config->universe_size) == CONFIG_FALSE) {
        config_destroy(cfg);
        return 1;
    }

    if (config_lookup_int(cfg, "universe.n_planets", &universe_config->n_planets) == CONFIG_FALSE) {
        config_destroy(cfg);
        return 1;
    }

    if (config_lookup_int(cfg, "universe.max_trash", &universe_config->max_trash) == CONFIG_FALSE) {
        config_destroy(cfg);
        return 1;
    }

    if (config_lookup_int(cfg, "universe.starting_trash", &universe_config->starting_trash) == CONFIG_FALSE) {
        config_destroy(cfg);
        return 1;
    }

    if (config_lookup_int(cfg, "universe.trash_ship_capacity", &universe_config->trash_ship_capacity) == CONFIG_FALSE) {
        config_destroy(cfg);
        return 1;
    }
    

    return 0;
}