#include <libconfig.h>
#include "universe-data.h"

int main() {
    //read config file
    config_t cfg;
    config_init(&cfg);


    if (config_read_file(&cfg, "./universe_config.conf") == CONFIG_FALSE) {
        config_destroy(&cfg);
        return 1;
    }

    //access config values
    UniverseConfig universe_config;
    read_config_file(&cfg, &universe_config);

    //close config file
    config_destroy(&cfg);

    return 0;
}