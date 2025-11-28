#include "physics-rules.h"
#include <SDL2/SDL.h>

void CheckEvents(int* running) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT) {
        printf("Quit event received. Exiting...\n");
        *running = 0;
    }
}

void UpdateUniverse() {
    // Placeholder for universe update logic
}

void Draw(SDL_Renderer* renderer) {    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Placeholder for drawing logic
}
