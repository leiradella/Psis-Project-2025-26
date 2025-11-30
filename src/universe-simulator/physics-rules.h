#ifndef PHYSICS_RULES_H
#define PHYSICS_RULES_H

#include <SDL2/SDL.h>

#include "universe-data.h"

//check events function
void CheckEvents(int* running, GameState* game_state);

//universe update function
void UpdateUniverse(GameState* game_state);

#endif
