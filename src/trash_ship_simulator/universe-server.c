#include <stdlib.h>
#include <SDL2/SDL.h>
#include <zmq.h>

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

    while (1)
    {
        int n;
        char buffer [2];

        int size = zmq_recv(pReceive, buffer, 1, 0);

        buffer[1] = '\0';

        printf("%s", buffer);
    }
    
}