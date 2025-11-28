#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "universe-data.h"

//draw function
void Draw(SDL_Renderer* renderer, Planet* planets, int n_planets);

#endif