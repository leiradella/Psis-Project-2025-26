#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <zmq.h>
#include "mylib.c"
#include "msg.pb-c.h"

Uint32 timer_callback(Uint32 interval, void *param){
    SDL_Event timer_event;
    printf("Timer callback function\n");

    SDL_zero(timer_event);
    timer_event.type = SDL_USEREVENT;
    timer_event.user.code = 2;
    timer_event.user.data1 = NULL;
    timer_event.user.data2 = NULL;
    SDL_PushEvent(&timer_event);
    return interval;
}

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

    u_int8_t msg = 0;

    Client protoMessage = CLIENT__INIT;

    SDL_TimerID timer_id = 0;
    timer_id = SDL_AddTimer(100, (SDL_TimerCallback)timer_callback, NULL);

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
                msg |= MYUP;
                break;
            
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                printf("LEFT -- Implement code\n");
                msg |= MYLEFT;
                break;

            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                printf("DOWN -- Implement code\n");
                msg |= MYDOWN;
                break;

            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                printf("RIGHT -- Implement code\n");
                msg |= MYRIGHT;
                break;

            default:
                printf("Key wasn't a direction.\n");
                break;

            }

        case SDL_USEREVENT:
            if(event.user.code == 2){
                //Add comunication
                printf("Escrever em protomsg.\n");
                protoMessage.ch.data = &msg;
                printf("Escrevi em protomsg.\n");
                protoMessage.ch.len = 1;
                int msg_len = client__get_packed_size(&protoMessage);
                uint8_t *buffer = (uint8_t *)malloc(msg_len);
                client__pack(&protoMessage, buffer);
                printf("Going to send message.\n");
                
                printf("%d", msg_len);
                
                zmq_send(sender, buffer, msg_len, 0);
                //printf("Message sent %c.\n", msg);
                msg = 0;
                free(buffer);

            }
            break;

        default:
            printf("Not a keypress.\n");
            break;
        }


    }

    printf("closing.\n");
    SDL_RemoveTimer(timer_id);
    closeContexts(programContext);

    return 0;
}