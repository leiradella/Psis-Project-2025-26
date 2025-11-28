#include "universe-data.h"
#include <libconfig.h>


int GetUniverseParameters(char* config_name, UniverseConfig* universe_config) {
    //init and read config file
    config_t cfg;
    config_init(&cfg);

    if (config_read_file(&cfg, config_name) == CONFIG_FALSE) {
        printf("Error in %s at line %d: %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return 1;
    }

    //read universe data from config file
    if (_ReadUniverseData(&cfg, universe_config) != 0) {
        config_destroy(&cfg);
        return 1;
    }

    config_destroy(&cfg);
    return 0;
}


int _LookupUniverseInt(config_t *cfg, int *value, const char* path) {
    if (config_lookup_int(cfg, path, value) == CONFIG_FALSE) {
        printf("Error in %s at line %d: %s\n", config_error_file(cfg), config_error_line(cfg), config_error_text(cfg));
        config_destroy(cfg);
        return 1;
    }
    return 0;
}


int _ReadUniverseData(config_t *cfg, UniverseConfig *universe_config) {

    //how many errors occurred
    int status = 0;

    status += _LookupUniverseInt(cfg, &universe_config->universe_size, "universe.size");
    status += _LookupUniverseInt(cfg, &universe_config->n_planets, "universe.n_planets");
    status += _LookupUniverseInt(cfg, &universe_config->max_trash, "universe.max_trash");
    status += _LookupUniverseInt(cfg, &universe_config->starting_trash, "universe.starting_trash");
    status += _LookupUniverseInt(cfg, &universe_config->trash_ship_capacity, "universe.trash_ship_capacity");
    return status;
}
