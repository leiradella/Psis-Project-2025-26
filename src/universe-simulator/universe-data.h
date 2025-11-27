#ifndef UNIVERSE_DATA_H
#define UNIVERSE_DATA_H

typedef struct UniverseConfig {
    int universe_size;
    int n_planets;
    int max_trash;
    int starting_trash;
    int trash_ship_capacity;
} UniverseConfig;

int read_config_file();



#endif