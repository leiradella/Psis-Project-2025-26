#include "physics-rules.h"
#include <SDL2/SDL.h>

void CheckEvents(int* running, GameState* game_state) {

    (void)game_state; //currently unused

    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        printf("Quit event received. Exiting...\n");
        *running = 0;
    }
}

void _NewTrashAcceleration(GameState* game_state) {
    Vector total_vector_force;
    Vector local_vector_force;

    for (int i = 0; i < game_state->n_trashes; i ++) {
        total_vector_force.amplitude = 0;
        total_vector_force.angle = 0;
        for (int n_planet = 0; n_planet < game_state->n_planets; n_planet ++){
            float force_vector_x = game_state->planets[n_planet].position.x - game_state->trashes[i].position.x;
            float force_vector_y = game_state->planets[n_planet].position.y - game_state->trashes[i].position.y;
            local_vector_force = MakeVector(force_vector_x, force_vector_y);
            local_vector_force.amplitude = game_state->planets[n_planet].mass / pow(local_vector_force.amplitude, 2);
            total_vector_force = AddVectors(local_vector_force, total_vector_force);
            game_state->trashes[i].acceleration = total_vector_force;
        }
    }
}

void _NewTrashVelocity(GameState* game_state) {
    for (int i = 0; i < game_state->n_trashes; i ++) {
        game_state->trashes[i].velocity.amplitude *= 0.99;
        game_state->trashes[i].velocity= AddVectors(game_state->trashes[i].velocity,
        game_state->trashes[i].acceleration);
    }
}

void _NewTrashPosition(GameState* game_state) {
    for (int i = 0; i < game_state->n_trashes; i ++) {
        
        float delta_x = game_state->trashes[i].velocity.amplitude * 
            cos(game_state->trashes[i].velocity.angle * (3.14159265f / 180.0f));

        float delta_y = game_state->trashes[i].velocity.amplitude * 
            sin(game_state->trashes[i].velocity.angle * (3.14159265f / 180.0f));

        game_state->trashes[i].position.x += delta_x;
        game_state->trashes[i].position.y += delta_y;

        //loop around the universe edges
        if (game_state->trashes[i].position.x < 0) {
            game_state->trashes[i].position.x += game_state->universe_size;
        }
        else if (game_state->trashes[i].position.x > game_state->universe_size) {
            game_state->trashes[i].position.x -= game_state->universe_size;
        }

        if (game_state->trashes[i].position.y < 0) {
            game_state->trashes[i].position.y += game_state->universe_size;
        }
        else if (game_state->trashes[i].position.y > game_state->universe_size) {
            game_state->trashes[i].position.y -= game_state->universe_size;
        }
    }
}

void _UpdatePlanets(GameState* game_state) {
    (void)game_state;
}

void _UpdateTrash(GameState* game_state) {

    _NewTrashAcceleration(game_state);
    _NewTrashVelocity(game_state);
    _NewTrashPosition(game_state);

}


void UpdateUniverse(GameState* game_state) {
    //update each object vector in the game state
    _UpdatePlanets(game_state);
    _UpdateTrash(game_state);
}
