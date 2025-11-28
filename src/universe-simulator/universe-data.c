#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <libconfig.h>

#include "universe-data.h"

int _GetUniverseParameters(const char* config_name, UniverseConfig* universe_config) {
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

int _IsValidPosition(float x, float y, float* points_x, float* points_y, int num_points, float min_dist) {
    for (int i = 0; i < num_points; i++) {
        float dx = points_x[i] - x;
        float dy = points_y[i] - y;
        float dist_sq = dx * dx + dy * dy;
        if (dist_sq < min_dist * min_dist) {
            return 0;
        }
    }
    return 1;
}

void _PositionPlanets(Planet* planets, int n_planets, int universe_size, int seed) {

    //coordinate variables
    float x, y;

    //loop flags
    int positioned = 0;
    int attempts = 0;
    const int max_attempts = 10 * n_planets; //max attempts to position all planets

    //current planet index
    int current_planet = 0;

    while (!positioned && attempts < max_attempts) {
        //get random coordinates within bounds (PLANET_RADIUS from edges)
        float range = universe_size - 2 * PLANET_RADIUS;
        x = PLANET_RADIUS + ((float)(rand_r((unsigned int*)&seed) % (int)range));
        y = PLANET_RADIUS + ((float)(rand_r((unsigned int*)&seed) % (int)range));

        //valid flag
        int valid = 1;

        //check if the position is valid
        for (int i = 0; i < n_planets; i++) {
            if (planets[i].x == -100.0f && planets[i].y == -100.0f) {
                //this planet is not placed yet, skip it
                continue;
            }

            float dx = planets[i].x - x;
            float dy = planets[i].y - y;
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq < MIN_PLANET_DISTANCE * MIN_PLANET_DISTANCE) {
                //position is invalid, break and try again
                valid = 0;
                break;
            }
        }

        if (valid) {
            //position is valid, assign it to the planet
            planets[current_planet].x = x;
            planets[current_planet].y = y;
            current_planet++;
        }

        if (current_planet >= n_planets) {
            //all planets positioned
            positioned = 1;
        }
    }
}

Planet *CreateInitialUniverseState(const char* config_name, int seed) {

    //create a universe config struct with the parameters
    UniverseConfig universe_config;

    //get the params with the config file
    _GetUniverseParameters(config_name, &universe_config);

    //now we can use universe_config to create the initial state of the universe
    //the results are semi deterministic based on the seed value

    //create a vector with all planets
    Planet* planets = malloc(universe_config.n_planets * sizeof(Planet));

    //initialize radius and mass constants, names, positions, and trash amounts
    for (int i = 0; i < universe_config.n_planets; i++) {
        planets[i].mass = PLANET_MASS;
        planets[i].radius = PLANET_RADIUS;
        planets[i].trash_amount = INITIAL_TRASH_AMOUNT;
        planets[i].name = 'A' + i; //assign names like A B C...

        //initialize positions with invalid values
        planets[i].x = -100.0f;
        planets[i].y = -100.0f;
    }
    
    //position all planets
    _PositionPlanets(planets, universe_config.n_planets, universe_config.universe_size, seed);

    //debug print all planets
    for (int i = 0; i < universe_config.n_planets; i++){
        printf("Planet %c: Position (%.2f, %.2f), Mass %d, Radius %.2f, Trash Amount %d\n",
               planets[i].name,
               planets[i].x,
               planets[i].y,
               planets[i].mass,
               planets[i].radius,
               planets[i].trash_amount);
    }

    return planets;
}

int GetNumberPlanets(const char* config_name) {

    int n_planets;

    config_t cfg;
    config_init(&cfg);
    if (config_read_file(&cfg, config_name) == CONFIG_FALSE) {
        printf("Error in %s at line %d: %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return 0;
    }

    //get the params with the config file
    _LookupUniverseInt(&cfg, &n_planets, "universe.n_planets");

    config_destroy(&cfg);

    return n_planets;
}
