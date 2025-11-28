#ifndef PHYSICS_RULES_H
#define PHYSICS_RULES_H

#include <SDL2/SDL.h>

#include "universe-data.h"

//check events function
void CheckEvents(int* running);

//universe update function
void UpdateUniverse(Planet* planets);

#endif
