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

        //planet collision
        if (game_state->n_trashes >= game_state->max_trash) {
            continue; //skip collision if at max trash
        }
        for (int j = 0; j < game_state->n_planets; j++) {
            float dx = game_state->trashes[i].position.x - game_state->planets[j].position.x;
            float dy = game_state->trashes[i].position.y - game_state->planets[j].position.y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance < COLLISION_DISTANCE) {
                //create 2 trashes from the collision (reuse the current trash and create a new one)
                
                //add a new trash child into this dog gone universe
                game_state->trashes[game_state->n_trashes] = game_state->trashes[i];
                game_state->n_trashes++;

                //slightly change both their angles based on the collision
                game_state->trashes[i].velocity.angle += 15.0f;
                game_state->trashes[game_state->n_trashes - 1].velocity.angle -= 15.0f;
            }
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

void _UpdateShips(GameState* game_state) {
    for (int i = 0; i < game_state->max_ships; i++) {
        if (!game_state->ships[i].is_active) {
            continue;
        }

        //update position based on direction
        switch (game_state->ships[i].direction) {
            case UP:
                game_state->ships[i].Position.y -= SHIP_SPEED;
                break;
            case DOWN:
                game_state->ships[i].Position.y += SHIP_SPEED;
                break;
            case LEFT:
                game_state->ships[i].Position.x -= SHIP_SPEED;
                break;
            case RIGHT:
                game_state->ships[i].Position.x += SHIP_SPEED;
                break;
            default:
                break;
        }


        //loop around the universe edges
        if (game_state->ships[i].Position.x < 0) {
            game_state->ships[i].Position.x += game_state->universe_size;
        }
        else if (game_state->ships[i].Position.x > game_state->universe_size) {
            game_state->ships[i].Position.x -= game_state->universe_size;
        }

        if (game_state->ships[i].Position.y < 0) {
            game_state->ships[i].Position.y += game_state->universe_size;
        }
        else if (game_state->ships[i].Position.y > game_state->universe_size) {
            game_state->ships[i].Position.y -= game_state->universe_size;
        }

        //check for trash collision
        for (int j = 0; j < game_state->n_trashes; j++) {
            float dx = game_state->ships[i].Position.x - game_state->trashes[j].position.x;
            float dy = game_state->ships[i].Position.y - game_state->trashes[j].position.y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance < game_state->ships[i].radius + game_state->trashes[j].radius) {
                //collect trash if ship has capacity
                if (game_state->ships[i].current_trash < game_state->trash_ship_capacity) {
                    game_state->ships[i].current_trash++;
                    //remove trash from universe by replacing it with the last trash
                    game_state->trashes[j] = game_state->trashes[game_state->n_trashes - 1];
                    game_state->n_trashes--;
                    j--; //check this index again as it has new trash now
                }
            }
        }

        //check for planet delivery (can only deliver to its own planet)
        for (int j = 0; j < game_state->n_planets; j++) {
            float dx = game_state->ships[i].Position.x - game_state->planets[j].position.x;
            float dy = game_state->ships[i].Position.y - game_state->planets[j].position.y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance < game_state->ships[i].radius + game_state->planets[j].radius) {

                //check if ship is at its own planet
                if (game_state->ships[i].planet_id == game_state->planets[j].name) {
                    //deliver all trash
                    game_state->planets[j].trash_amount += game_state->ships[i].current_trash;
                    game_state->ships[i].current_trash = 0;
                    break;
                } else {
                    if (game_state->ships[i].current_trash == 0) {
                        break; //no trash to spill
                    }

                    //crash into wrong planet, making more trash
                    int trash_created = game_state->ships[i].current_trash + 1;

                    //spill all trash and create an extra one, in random directions
                    for (int k = 0; k < trash_created; k++) {
                        if (game_state->n_trashes >= game_state->max_trash) {
                            break; //stop creating trash if at max
                        }

                        float angle = (float)(rand() % 360);

                        Trash new_trash;
                        new_trash.position.x = (float)(rand() % game_state->universe_size);
                        new_trash.position.y = (float)(rand() % game_state->universe_size);
                        new_trash.velocity.amplitude = 0.0f;
                        new_trash.velocity.angle = 0.0f;
                        new_trash.acceleration.amplitude = 0.0f;
                        new_trash.acceleration.angle = 0.0f;
                        new_trash.mass = TRASH_MASS;
                        new_trash.radius = TRASH_RADIUS;
                        

                        game_state->trashes[game_state->n_trashes] = new_trash;
                        game_state->n_trashes++;
                    }

                    game_state->ships[i].current_trash = 0;

                    break;
                }
            }
        }
    }
}

void _CheckGameOver(GameState* game_state) {

    //once the trash hits the max, its game over
    //were gonna make a text appear on screen
    if (game_state->n_trashes >= game_state->max_trash) {
        game_state->is_game_over = 1;
    }
}

void UpdateUniverse(GameState* game_state, int trashMovement) {
    //update each object vector in the game state
    _UpdatePlanets(game_state);
    if(trashMovement)
        _UpdateTrash(game_state);

    _UpdateShips(game_state);

    _CheckGameOver(game_state);
}
