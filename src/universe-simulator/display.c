#include "display.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void Draw(SDL_Renderer* renderer, Planet* planets, int n_planets) {

    //load font for planet names
    TTF_Font* font = TTF_OpenFont("arial.ttf", 12);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    //set background color to whiteish gray
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    //planets will be drawn as just circles with their names on the top right
    for (int i = 0; i < n_planets; i++) {
        //draw planet i as a filled circle (blue)
        filledCircleRGBA(renderer, (int)planets[i].position.x, (int)planets[i].position.y, (int)planets[i].radius, 0, 0, 255, 255);

        //draw planet name at top-right of planet
        //name is a single char + the amount of trash inside the planet
        char name_text[10];
        snprintf(name_text, sizeof(name_text), "%c%d", planets[i].name, planets[i].trash_amount);
        SDL_Color textColor = {0, 0, 0, 255}; //black color
        
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, name_text, textColor);
        if (textSurface == NULL) {
            printf("Failed to create text surface: %s\n", TTF_GetError());
            exit(1);
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture == NULL) {
            printf("Failed to create text texture: %s\n", SDL_GetError());
            SDL_FreeSurface(textSurface);
            exit(1);
        }

        SDL_Rect textRect;
        textRect.x = (int)(planets[i].position.x + planets[i].radius);
        textRect.y = (int)(planets[i].position.y - planets[i].radius);
        textRect.w = textSurface->w * 2;  
        textRect.h = textSurface->h * 2;
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
    }

    //present the rendered frame
    SDL_RenderPresent(renderer);
}