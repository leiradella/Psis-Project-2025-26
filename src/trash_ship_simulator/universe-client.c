#include <stdlib.h>
#include <SDL2/SDL.h>
#include <zmq.h>
#include "mylib.c"

int main(int argc, char *argv[])
{

    //Create a simple client that reads the cursor keys

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    struct contextDataforClosing *programContext = createContextDataforClosing(SDL_Quit, NULL, NULL);

    if(!programContext) return -1;

    SDL_Window *pWin = SDL_CreateWindow("ShipClient",
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED,
                                        1000, 1000, 0);

    if(!pWin){
        printf("error initializing SDL: %s\n", SDL_GetError());
        closeContexts(programContext);
        return -1;
    }

    programContext = createContextDataforClosing(SDL_DestroyWindow, pWin, programContext);

    if(!programContext) return -1;

    //Create ZMQ_Context
    void *pContext = zmq_ctx_new();

    if(!pContext){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        closeContexts(programContext);
        return -1;
    }

    programContext = createContextDataforClosing(zmq_ctx_destroy, pContext, programContext);

    if(!programContext) return -1;

    //Socket to send messages to
    void *sender = zmq_socket(pContext, ZMQ_PUSH);
    zmq_connect(sender, "tcp://localhost:5558");

    if(!sender){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        closeContexts(programContext);
        return -1;
    }

    programContext = createContextDataforClosing(zmq_close, sender, programContext);

    if(!programContext) return -1;

    int close = 0;
    char msg;

    while(!close)
    {
        SDL_Event event;
        
        //Controlo de eventos
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            close = 1;
            break;
        
        case SDL_KEYDOWN:
            //Identificação da tecla
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                printf("UP -- Implement code\n");
                msg = 'u';
                break;
            
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                printf("LEFT -- Implement code\n");
                msg = 'l';
                break;

            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                printf("DOWN -- Implement code\n");
                msg = 'd';
                break;

            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                printf("RIGHT -- Implement code\n");
                msg = 'r';
                break;

            default:
                printf("Key wasn't a direction.\n");
                break;

            }
        default:
            printf("Not a keypress.\n");
            break;
        }

        //Add comunication
        printf("Going to send message.\n");
        zmq_send(sender, &msg, 1, 0);
        printf("Message sent.\n");
    }

    printf("closing.\n");
    closeContexts(programContext);

    return 0;
}