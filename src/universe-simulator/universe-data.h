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

//planet structure
typedef struct Planet {
    char name;
    int x;
    int y;
    int trash_amount;
} Planet;

//this function reads universe parameters from the config file with name config_name
//and stores them in universe_config
//this function is the only public function related to universe data reading
int GetUniverseParameters(char* config_name, UniverseConfig* universe_config);

//this function looks up and integer from the config file cfg with name path and stores it in value
//returns 0 on success, 1 on failure printing an error message if any occurs
int _LookupUniverseInt(config_t *cfg, int *value, const char* path);

//this function reads all universe data from the config file cfg and stores it in universe_config
//returns the number of errors occurred during the process
int _ReadUniverseData(config_t *cfg, UniverseConfig *universe_config);

#endif
