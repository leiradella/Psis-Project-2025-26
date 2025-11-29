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

//universe objects and other structures

//position structure
typedef struct Position {
    float x;
    float y;
} Position;

//vector structure (used for velocities)
typedef struct Vector {
    float amplitude;
    float angle;
} Vector;

//planet structure
typedef struct Planet {
    char name;
    Position position;
    int mass;
    float radius;
    int trash_amount;
} Planet;

//trash structure
typedef struct Trash {
    Position position;
    Vector velocity;
    int mass;
    float radius;
} Trash;

#define PLANET_MASS 10
#define PLANET_RADIUS 20.0f
#define INITIAL_TRASH_AMOUNT 0
#define MIN_PLANET_DISTANCE 80.0f

//this function reads universe parameters from the config file with name config_name
//and stores them in universe_config
int _GetUniverseParameters(const char* config_name, UniverseConfig* universe_config);

//this function looks up and integer from the config file cfg with name path and stores it in value
//returns 0 on success, 1 on failure printing an error message if any occurs
int _LookupUniverseInt(config_t *cfg, int *value, const char* path);

//this function reads all universe data from the config file cfg and stores it in universe_config
//returns the number of errors occurred during the process
int _ReadUniverseData(config_t *cfg, UniverseConfig *universe_config);

//helper to check if a point is valid (far enough from all existing points)
int _IsValidPosition(float x, float y, float* points_x, float* points_y, int num_points, float min_dist);

//position all planets randomly within the universe size using the seed value
//while making sure it does not overlap with other planets
//this function receives the vector with all planets to check for overlaps
//planets not placed yet have invalid positions (-100.0f, -100.0f)
void _PositionPlanets(Planet* planets, int n_planets, int universe_size, int seed);

//create the universe initial state using the universe configuration
//this function is the only public function related to universe data reading
Planet* CreateInitialUniverseState(const char* config_name, int seed);

//get number of planets from config file
int GetNumberPlanets(const char* config_name);

#endif
