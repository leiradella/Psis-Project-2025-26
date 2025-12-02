#include <stdlib.h>
#include <SDL2/SDL.h>
#include <zmq.h>
#include "mylib.c"
#include "msg.pb-c.h"

int main(int argc, char *argv[])
{
    //Create ZMQ_Context
    void *pContext = zmq_ctx_new();

    if(!pContext){
        printf("error initializing ZMQ: %s\n", zmq_strerror(errno));
        return -1;
    }

    struct contextDataforClosing *programContext = createContextDataforClosing(zmq_ctx_destroy, pContext, NULL);

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

        //buffer[1] = '\0';

        //printf("%b\n", msg);
    }while(1);
    
    closeContexts(programContext);
    return 0;
    
}