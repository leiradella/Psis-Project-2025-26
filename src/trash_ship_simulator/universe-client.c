#include <stdlib.h>
#include <SDL2/SDL.h>
#include <zmq.h>

int main(int argc, char *argv[])
{

    //Create a simple client that reads the cursor keys

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *pWin = SDL_CreateWindow("ShipClient",
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED,
                                        1000, 1000, 0);

    if(!pWin){
        printf("error initializing SDL: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    //Create ZMQ_Context
    void *context = zmq_ctx_new();

    if(!context){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        SDL_DestroyWindow(pWin);
        SDL_Quit();
        return -1;
    }

    //Socket to send messages to
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sender, "tcp://localhost:5558");

    if(!sender){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        zmq_ctx_destroy(context);
        SDL_DestroyWindow(pWin);
        SDL_Quit();
    }

    int close = 0;
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
                break;
            
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                printf("LEFT -- Implement code\n");
                break;

            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                printf("DOWN -- Implement code\n");
                break;

            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                printf("RIGHT -- Implement code\n");
                break;

            default:
                break;
            }
        default:
            break;
        }

        //Add comunication
        
    }

    zmq_close(sender);

    zmq_ctx_destroy(context);

    SDL_DestroyWindow(pWin);

    SDL_Quit();

    return 0;
}