#ifndef PHYSICS_RULES_H
#define PHYSICS_RULES_H

#include <SDL2/SDL.h>

//check events function
void CheckEvents(int* running);

//universe update function
void UpdateUniverse();

//draw function
void Draw(SDL_Renderer* renderer);

#endif