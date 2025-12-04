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
    //****************************************+
    //Prepare for SDL and ZMQ

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
                                        100, 100, 0);

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
    void *sender = zmq_socket(pContext, ZMQ_REQ);
    zmq_connect(sender, "tcp://localhost:5556");

    if(!sender){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        closeContexts(programContext);
        return -1;
    }

    programContext = createContextDataforClosing(zmq_close, sender, programContext);

    if(!programContext) return -1;


    //Program Start
    int close;


    Client protoMessage = CLIENT__INIT;

    //Get a ship id
        //Send a request for a ship
    u_int8_t msg = GETID;
    protoMessage.ch.data = &msg;
    protoMessage.ch.len = 1;
    int msg_len = client__get_packed_size(&protoMessage);
    uint8_t *buffer = (uint8_t *)malloc(msg_len);
    client__pack(&protoMessage, buffer);
    printf("Sent request for ID of size %d.\n", msg_len);

    zmq_send(sender, buffer, msg_len, 0);

        //Process server reponse to ship request
    zmq_recv(sender, buffer, msg_len, 0);
    Client *pProtoMessage = client__unpack(NULL, msg_len, buffer);
    msg = *((pProtoMessage -> ch.data));
    client__free_unpacked(pProtoMessage,NULL);
    uint8_t myID = msg & IDMASK;

    if(myID == GETID){
        printf("Invalid Ship id %d.\nClosing.\n", myID);
        close = 1;
    }else {
        printf("Received ship id %d.\n", myID);
        close = 0;
    }

    //Start controling the ship

    while(!close)
    {
        SDL_Event event;
        
        //Controlo de eventos
        while(SDL_PollEvent(&event) != 0){
            if (event.type == SDL_QUIT)
            {
                close = 1;
                break;
            }
        }
        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
        if (close == 1){
            msg = ISDC;
        } else if(currentKeyStates[SDL_SCANCODE_W]||currentKeyStates[SDL_SCANCODE_UP]){
            msg = MYUP;
        } else if(currentKeyStates[SDL_SCANCODE_A]||currentKeyStates[SDL_SCANCODE_LEFT]){
            msg = MYLEFT;
        } else if(currentKeyStates[SDL_SCANCODE_S]||currentKeyStates[SDL_SCANCODE_DOWN]){
            msg = MYDOWN;
        } else if(currentKeyStates[SDL_SCANCODE_D]||currentKeyStates[SDL_SCANCODE_RIGHT]){
            msg = MYRIGHT;
        } else {
            msg = MYSTILL;
        }
        
        printf("Information to send is %d.\n", msg);
        //Add comunication
        msg |= myID;
        protoMessage.ch.data = &msg;
        protoMessage.ch.len = 1;
        client__pack(&protoMessage, buffer);
                
        zmq_send(sender, buffer, msg_len, 0);
        printf("Message sent %d.\n", msg);

        zmq_recv(sender, buffer, msg_len, 0);
    }

    printf("closing.\n");
    free(buffer);
    closeContexts(programContext);

    return 0;
}