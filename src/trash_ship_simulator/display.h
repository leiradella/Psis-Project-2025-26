#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "universe-data.h"
#include "graceful-exit.h"

//draw function
void Draw(SDL_Renderer* renderer, GameState* game_state);

//Poll all events to check for SDL_QUIT
int checkQuit();

//Check KeyboardState to see if a movement key was pressed
uint8_t checkKeyboard(int close);

//The safe class functions are wrapped version of their name
//namesakes and handle graceful-exit data
SDL_Window *safe_SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint8 flags, gful_lifo **gracefull_lifo);
void safe_SDL_Init(Uint32 flags, gful_lifo **gracefull_lifo);
SDL_Renderer *safe_SDL_CreateRenderer(  SDL_Window * window,
                                        int index, Uint32 flags,
                                        gful_lifo **graceful_lifo);

void safe_TTF_Init(gful_lifo **graceful_lifo);

//Create the program specific Windows
SDL_Window *createClientWindow(gful_lifo **gracefull_lifo);
SDL_Window *createServerWindow(GameState *game_state, gful_lifo **gracefull_lifo);

//Create server specific renderer
SDL_Renderer *createServerRenderer(SDL_Window *window, gful_lifo **gracefull_life);

#endif