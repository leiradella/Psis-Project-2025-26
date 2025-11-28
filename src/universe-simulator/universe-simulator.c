#include <libconfig.h>
#include <SDL2/SDL.h>

#include "universe-data.h"
#include "physics-rules.h"

int main() {

    //create a universe config struct with the parameters
    UniverseConfig universe_config;

    //get the params with the GetUniverseParameters function (errors are handled inside the function)
    if (GetUniverseParameters("universe_config.conf", &universe_config) != 0) {
        return 1;
    }

    //Initalize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    //create SDL_window variable
    SDL_Window *window = SDL_CreateWindow(
        "Universe Simulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        800, 
        600, 
        SDL_WINDOW_SHOWN
    );

    //check if window was created successfully
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //create SDL_renderer variable
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //main loop flag
    int running = 1;

    //main loop
    while (running) {
        CheckEvents(&running);
        UpdateUniverse();
        Draw(renderer);
    }

    //destroy SDL variables and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}