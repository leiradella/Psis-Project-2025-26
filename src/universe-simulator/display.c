#include "display.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void Draw(SDL_Renderer* renderer, Planet* planets, int n_planets) {

    //load font for planet names
    TTF_Font* font = TTF_OpenFont("SuperChiby.ttf", 12);
    (void)font; //currently unused

    //set background color to whiteish gray
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    //planets will be drawn as just circles with their names on the top right
    for (int i = 0; i < n_planets; i++) {
        //draw planet i as a filled circle (blue)
        filledCircleRGBA(renderer, (int)planets[i].x, (int)planets[i].y, (int)planets[i].radius, 0, 0, 255, 255);

        //draw planet name at top-right of planet
    }

    //present the rendered frame
    SDL_RenderPresent(renderer);
}