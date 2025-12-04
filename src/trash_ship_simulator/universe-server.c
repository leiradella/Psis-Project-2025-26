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
    void *pReceive = zmq_socket(pContext, ZMQ_REP);
    zmq_bind(pReceive, "tcp://*:5556");

    if(!pReceive){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        closeContexts(programContext);
        return -1;
    }

    programContext = createContextDataforClosing(zmq_close, pReceive, programContext);

    if(!programContext) return -1;

    Player *firstPlayer = NULL;
    

    u_int8_t msg;
    zmq_msg_t zmq_msg;
    zmq_msg_init(&zmq_msg);
    uint8_t *buffer = (uint8_t *)malloc(MSGLEN);


    Player *currPlayer;
    uint8_t newID;
    uint8_t spaceships[20];

    int nNewPlayers=1;
    int running = 1;
    int spaceshipNum;

    for(spaceshipNum = 0; spaceshipNum<20;spaceshipNum++)
        spaceships[spaceshipNum] = 0;

    while(running)
    {
        for(int i = 0; i < game_state->n_ships + nNewPlayers; i++){
            printf("Waiting for Message\n");
            int msg_len = zmq_recvmsg(pReceive, &zmq_msg , 0);
            if(msg_len == -1){
                printf("Error getting ZMQ message: %s\n", zmq_strerror(errno));
                return -1;
            }
            printf("Received message of lenght %d.\n", msg_len);
            void *msg_data = zmq_msg_data(&zmq_msg);
            printf("Retrieved data stored at %p.\n", msg_data);
            Client *protoMessageRecv = client__unpack(NULL, msg_len, msg_data);
            printf("Stored information unpacked to %p.\n", protoMessageRecv);

            if(protoMessageRecv){
                printf("Valid unnpack address.\n");
                msg = *(protoMessageRecv->ch.data);
                client__free_unpacked(protoMessageRecv, NULL);
                
                if(msg == GETID){
                    //Handling new player requests
                    printf("Received a GETID message.\n");
                    printf("Number of ships is %d.\n", game_state->n_ships);
                    switch (game_state->n_ships)
                    {
                        //If Player list is full
                    case 20:
                        msg = MAXPLAYERS;
                        nNewPlayers++;
                        break;

                        //If player list is partialy filled
                    default:
                        //Get an unused ID
                        newID = (uint8_t)(rand() % 31);
                        for(currPlayer = firstPlayer; currPlayer; newID = (uint8_t)(rand() % 31))
                            for(currPlayer = firstPlayer; currPlayer->ID != newID; currPlayer = currPlayer -> nextPlayer)
                                if(currPlayer) break;
                        
                        game_state ->n_ships++;
                        //Create a new player entity
                        Player *newPlayer = malloc(sizeof(Player));
                        //Get an unused ship number
                        for(spaceshipNum = 0; spaceships[spaceshipNum]==1; spaceshipNum++);
                        //Set unused ship as in use
                        spaceships[spaceshipNum] = 1;
                        //Create new player entity with the spaceship number considering there is no spaceship 0.
                        *newPlayer = (Player){newID, spaceshipNum+1, NULL};
                        for(currPlayer = firstPlayer; currPlayer->nextPlayer != NULL; currPlayer = currPlayer->nextPlayer);
                        currPlayer->nextPlayer = newPlayer;
                        msg = newID << 3;
                        break;

                        //If Player list is empty
                    case 0:
                        //Fill the first entity
                        spaceships[0] = 1;
                        newID = (uint8_t)(rand() % 31);
                        game_state->n_ships++;
                        nNewPlayers--;
                        firstPlayer = malloc(sizeof(Player));
                        if(!firstPlayer) return -1;

                        *firstPlayer = (Player){newID, game_state -> n_ships, NULL};
                        msg = newID << 3;
                        break;
                    }
                }else{
                    printf("Not a getid msg.\n");
                    //Handling all other messages
                    //Get the sender ID
                    int8_t msgSenderID = (msg & IDMASK) >> 3;
                    
                    //Received a message to Exit the game
                    if((msg & CODEMASK) == 7){
                        printf("Received a exit message.\n");

                        //Remove Player entity with corresponding id from player list.
                            //Verificar que quem mandou a mensagem pertence à player list,
                            //se pertencer guardar o pointer para o player
                        currPlayer = firstPlayer;
                        printf("First player id %d, message sender id %d.\n", firstPlayer->ID, msgSenderID);
                        while(currPlayer != NULL){
                            if(currPlayer->ID == msgSenderID) break;
                            currPlayer = currPlayer->nextPlayer;
                        }
                        printf("Endereço na player list da pessoa que pediu para sair %p.\n", currPlayer);
                            //Atualizar a lista de player para não conter o ex-player e libertar a menória por ele ocupada
                        if (currPlayer != NULL){
                            Player *temp = currPlayer;
                            if(currPlayer == firstPlayer){
                                firstPlayer = firstPlayer->nextPlayer;
                            }else{
                                for(currPlayer = firstPlayer; currPlayer->nextPlayer->ID != msgSenderID; currPlayer = currPlayer->nextPlayer);
                                currPlayer->nextPlayer = temp->nextPlayer;
                            }
                            spaceships[temp->shipID - 1] = 0;
                            free(temp);
                            msg = SUCCESS;
                            game_state->n_ships--;

                            printf("Finished updating msg list.\n");
                        } else {
                            //Message sender was not in player list
                            msg = FAIL;
                            
                        }
                        //Add temporary players so this loop does't skip last attended client 
                        //(note: if msg wasnt from client it works because it ignores the face 
                        //user and if a user was destroyed it doesn't shorten the get all client messages loop before intended)
                        nNewPlayers++;

                    }
                    //Received a message to not move the spaceship
                    else if ((msg & CODEMASK) == 4){
                        printf("Received a NOMOVE message.\n");
                        //Colocar na informação da spaceship speed 0
                            //Verificar que quem mandou a mensagem pertence à player list,
                            //se pertencer guardar o pointer para o player
                        currPlayer = firstPlayer;
                        while(currPlayer != NULL){
                            if(currPlayer->ID == msgSenderID) break;
                            currPlayer = currPlayer->nextPlayer;
                        }
                        if(currPlayer)
                        {
                            spaceshipNum = currPlayer->shipID;
                            game_state->planets[spaceshipNum].ship.velocity.amplitude=0;
                            msg = SUCCESS;
                        }else{
                            //Player Id doesn't exist
                            msg = FAIL;
                            nNewPlayers++;
                        }
                        

                    //Received a message to move a spaceship
                    } else if ((msg & CODEMASK) < 4){
                        printf("Received a MOVE message.\n");
                        //Colocar na informação da spaceship speed e angle
                            //Verificar que quem mandou a mensagem pertence à player list,
                            //se pertencer guardar o pointer para o player
                        currPlayer = firstPlayer;
                        while(currPlayer != NULL){
                            if(currPlayer->ID == msgSenderID) break;
                            currPlayer = currPlayer->nextPlayer;
                        }
                        if(currPlayer)
                        {
                            spaceshipNum = currPlayer->shipID;
                            game_state->planets[spaceshipNum].ship.velocity.amplitude=SPACESHIPSPEED;
                            switch (msg & CODEMASK)
                            {
                            case MYUP:
                                game_state->planets[spaceshipNum].ship.velocity.angle = ANGLEUP;
                                break;

                            case MYLEFT:
                                game_state->planets[spaceshipNum].ship.velocity.angle = ANGLELEFT;
                                break;

                            case MYDOWN:
                                game_state->planets[spaceshipNum].ship.velocity.angle = ANGLEDOWN;
                                break;

                            default:
                                game_state->planets[spaceshipNum].ship.velocity.angle = ANGLERIGHT;
                                break;
                            }
                            printf("Corrigir ângulos? universe.server linha 250 \n");
                            msg = SUCCESS;
                        }else{
                            //Player Id doesn't exist
                            msg = FAIL;
                            nNewPlayers++;
                        }
                    }

                }

            }else {
                printf("Invalid message unpack adress.\n");
                client__free_unpacked(protoMessageRecv, NULL);
                msg = 0; //Failed to read message
                nNewPlayers++;
            }
            
            Client protoMessageSend = CLIENT__INIT;
            protoMessageSend.ch.data = &msg;
            protoMessageSend.ch.len = 1;

            client__pack(&protoMessageSend, buffer);
            
            zmq_send(pReceive, buffer, MSGLEN, 0);
            printf("Sent response to client.\n");
            printf("i = %d number of messages to read = %d\n", i, game_state->n_ships + nNewPlayers);
            SDL_Delay(500);

            if(game_state->n_ships == 0) nNewPlayers = 1;

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