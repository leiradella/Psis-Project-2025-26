#include "physics-rules.h"
#include <SDL2/SDL.h>

void CheckEvents(int* running, GameState* game_state) {

    (void)game_state; //currently unused

    SDL_Event event;
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT) {
        printf("Quit event received. Exiting...\n");
        *running = 0;
    }
}

void UpdateUniverse(GameState* game_state) {
    (void)game_state; //currently unused
}
