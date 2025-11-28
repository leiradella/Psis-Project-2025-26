#include "physics-rules.h"
#include <SDL2/SDL.h>

void CheckEvents(int* running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            printf("Quit event received. Exiting...\n");
            *running = 0;
        }
    }
}

void UpdateUniverse() {
    // Placeholder for universe update logic
}

void Draw(SDL_Renderer* renderer) {    
    // Placeholder for drawing logic
}