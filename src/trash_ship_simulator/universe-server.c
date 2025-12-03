#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <zmq.h>
#include "msg.pb-c.h"


#include "universe-data.h"
#include "physics-rules.h"
#include "display.h"

#include "mylib.c"      //Adicionar a outras bibliotecas.


int main(int argc, char *argv[])
{

    //seed random number generator with current time
    srand(time(NULL));
    int seed = rand(); //initialize seed with a random value

    //create the universal initial state here using universe_config parameters
    GameState* game_state = CreateInitialUniverseState("universe_config.conf", seed);

    if (game_state == NULL) {
        printf("Failed to create initial universe state. Exiting...\n");
        return 1;
    }

    //Initalize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    struct contextDataforClosing *programContext = createContextDataforClosing(SDL_Quit, NULL, NULL);

    if(!programContext) return -1;


    //Initialize SDL_ttf
    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        closeContexts(programContext);
        return 1;
    }

        //create SDL_window variable
    SDL_Window *window = SDL_CreateWindow(
        "Universe Simulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        game_state->universe_size, 
        game_state->universe_size, 
        SDL_WINDOW_SHOWN
    );

    //check if window was created successfully
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        closeContexts(programContext);
        return 1;
    }

    programContext = createContextDataforClosing(SDL_DestroyWindow, window, programContext);

    if(!programContext) return -1;

    //create SDL_renderer variable
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        closeContexts(programContext);
        return 1;
    }
    programContext = createContextDataforClosing(SDL_DestroyRenderer, renderer, programContext);

    if(!programContext) return -1;


    //Create ZMQ_Context
    void *pContext = zmq_ctx_new();

    if(!pContext){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        return -1;
    }

    programContext = createContextDataforClosing(zmq_ctx_destroy, pContext, programContext);

    if(!programContext) return -1;

    //Socket to send messages to
    void *pReceive = zmq_socket(pContext, ZMQ_PULL);
    zmq_bind(pReceive, "tcp://localhost:5558");

    if(!pReceive){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        closeContexts(programContext);
        return -1;
    }

    programContext = createContextDataforClosing(zmq_close, pReceive, programContext);

    if(!programContext) return -1;

    zmq_msg_t zmq_msg;
    zmq_msg_init(&zmq_msg);
    u_int8_t msg;

    int running = 1;
    do
    {
        msg = 0;
        printf("Waiting for message.\n");
        int size = zmq_recvmsg(pReceive, &zmq_msg, 0);
        printf("Received message of size %d.\n", size);
        void *msg_data = zmq_msg_data(&zmq_msg);
        printf("Saved Message Data to %p.\n", msg_data);
        Client *protoMessage = client__unpack(NULL, size, msg_data);
        printf("Unpacked Message.\n");
        printf("%p\n", protoMessage);
        if(protoMessage){
            printf("Entered if.\n");
            msg = *(protoMessage->ch.data);
            printf("Copied message content. %d\n", msg);
            client__free_unpacked(protoMessage, NULL);

        }

        //CheckEvents(&running, game_state)

    }while(running);
    
    closeContexts(programContext);
    return 0;
    
}