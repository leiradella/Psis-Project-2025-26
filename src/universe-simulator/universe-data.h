#ifndef UNIVERSE_DATA_H
#define UNIVERSE_DATA_H

#include <libconfig.h>

//unvierse configuration structure
typedef struct UniverseConfig {
    int universe_size;
    int n_planets;
    int max_trash;
    int starting_trash;
    int trash_ship_capacity;
} UniverseConfig;

int GetUniverseParameters(char* config_name, UniverseConfig* universe_config);
int _ReadUniverseData(config_t *cfg, UniverseConfig *universe_config);



#endif