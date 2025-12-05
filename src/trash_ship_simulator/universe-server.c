#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <zmq.h>
#include "msg.pb-c.h"


#include "universe-data.h"
#include "physics-rules.h"
#include "display.h"

#include "Communication.h"      //Adicionar a outras bibliotecas.

#define DOESTRASHMOVE 0     //0 for no and 1 for yes

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

    gful_lifo *programContext = GFUL_INIT;
    
    //Init SDL
        safe_SDL_Init(SDL_INIT_EVERYTHING, &programContext);
        safe_TTF_Init(&programContext);
        SDL_Window *window = createServerWindow(game_state, &programContext);
        SDL_Renderer *renderer = createServerRenderer(window, &programContext);


    //Init ZMQ
        void *pContext = safe_zmq_ctx_new(&programContext);
        void *pReceive = safe_zmq_socket(pContext, ZMQ_REP, &programContext);
        safe_zmq_bind(pReceive, "tcp://*:5556", &programContext);


    /*****************************+*/
    /*Program*/

    Player *firstPlayer = NULL;

    u_int8_t msg;
    zmq_msg_t zmq_msg;
    zmq_msg_init(&zmq_msg);
    uint8_t *buffer = (uint8_t *)malloc(MSGLEN);
    createContextDataforClosing(free, buffer, &programContext);

    Player *currPlayer;
    uint8_t newID;

    int nNewPlayers=1;
    int running = 1;
    int spaceshipNum;

    while(running)
    {
        for(int i = 0; i < game_state->n_ships + nNewPlayers; i++){
            printf("Waiting for Message\n");
            Client *protoMessageRecv = serverReceive(pReceive, &zmq_msg, 0, &programContext);
            if(protoMessageRecv){
                msg = *(protoMessageRecv->ch.data);
                client__free_unpacked(protoMessageRecv, NULL);
                
                if(msg == GETID){
                    msg = handleNewClient(&firstPlayer, &nNewPlayers, game_state, programContext);
                }else{
                    uint8_t msgSenderID = (msg & IDMASK) >> 3;
                    //Received a message to Exit the game
                    if((msg & CODEMASK) == 7){
                        msg = handleClientExit( &firstPlayer, msgSenderID, 
                                                &nNewPlayers, game_state);
                      }
                    else if ((msg & CODEMASK) == 4){
                        msg = handleClientStill(firstPlayer, msgSenderID,game_state, &nNewPlayers);

                    //Received a message to move a spaceship
                    } else if ((msg & CODEMASK) < 4){
                        msg = handleClientMove(firstPlayer, msgSenderID, game_state, &nNewPlayers, msg);
                    }
                }

            }else {
                printf("Invalid message unpack adress.\n");
                //client__free_unpacked(protoMessageRecv, NULL);
                msg = 0; //Failed to read message
                nNewPlayers++;
            }
            serverSend(msg, pReceive, &programContext);
            // SDL_Delay(500); //for debbuging

            if(game_state->n_ships == 0) nNewPlayers = 1;
            else nNewPlayers = 0;

        }


        CheckEvents(&running, game_state);
        UpdateUniverse(game_state, DOESTRASHMOVE);
        Draw(renderer, game_state);

        SDL_Delay(16);
    }
    
    //Clear Player list
    currPlayer = firstPlayer;
    while(currPlayer){
        Player *temp = currPlayer;
        currPlayer = currPlayer->nextPlayer;
        free(temp);
    }

    zmq_msg_close(&zmq_msg);
    free(buffer);
    closeContexts(programContext);
    return 0;
    
}