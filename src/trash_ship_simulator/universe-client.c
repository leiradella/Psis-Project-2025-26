#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <zmq.h>

#include "Communication.h"
#include "msg.pb-c.h"
#include "display.h"

int main(int argc, char *argv[])
{
    //****************************************+
    //Prepare for SDL and ZMQ
    gful_lifo *programContext = GFUL_INIT;

        //Prepare SDL
            safe_SDL_Init(SDL_INIT_EVERYTHING, &programContext);
            SDL_Window *pWin = createClientWindow(&programContext);

        //Prepare ZMQ
            void *pContext = safe_zmq_ctx_new(&programContext);
            void *sender = safe_zmq_socket(pContext, ZMQ_REQ, &programContext);
            safe_zmq_connect(sender, "tcp://localhost:5556", &programContext);

    //Program Start
    int close = 0;
    Client protoMessage = CLIENT__INIT;

    //Get a ship id
    int myID = join_request(sender, protoMessage, &programContext);

    //Start controling the ship
    while(!close)
    {   
        close = checkQuit();

        uint8_t msg = checkKeyboard(close);
        
        //Add comunication
        initCntrlMsg(&msg, myID, &protoMessage);
        
        send_cntrl(sender, protoMessage, &programContext);
    }

    printf("closing.\n");
    closeContexts(programContext);

    return 0;
}