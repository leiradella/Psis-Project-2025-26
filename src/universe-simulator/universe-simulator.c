#include <stdlib.h>

#include <libconfig.h>
#include <SDL2/SDL.h>

#include "universe-data.h"
#include "physics-rules.h"
#include "display.h"

int main() {

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

    int seed = rand(); //initialize seed with a random value

    //create the universal initial state here using universe_config parameters
    Planet* planets = CreateInitialUniverseState("universe_config.conf", seed);
    int n_planets = GetNumberPlanets("universe_config.conf");

    //main loop: check events, update universe state, draw universe
    while (running) {
        CheckEvents(&running);
        UpdateUniverse(planets);
        Draw(renderer, planets, n_planets);
    }

    //free allocated universe state memory
    free(planets);

    //destroy SDL variables and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Universe Simulator exited successfully.\n");
    return 0;
}
