#ifndef MYLIB_H

#define MYLIB_H


//Descriptors of message content

#define GETID   248         //For server

#define MAXPLAYERS  248     //For client
#define SUCCESS     1       
#define FAIL        0 

#define ISMOVE  0
#define ISDC    7
#define MYSTILL 4

#define MYLEFT  1
#define MYUP    0
#define MYDOWN  2
#define MYRIGHT 3

#define IDMASK  248
#define CODEMASK 7

#define LASTSHIPID 19

//Other constants

#define SPACESHIPSPEED 10
#define ANGLEUP 0
#define ANGLELEFT PI / 2
#define ANGLEDOWN PI
#define ANGLERIGHT 3 * PI / 2

#define MSGLEN 3



struct contextDataforClosing{
    void (*pFunction)(void *);
    void *pArguments;
    struct contextDataforClosing *pPreviousStruct;
};

void closeContexts(struct contextDataforClosing *);

struct contextDataforClosing *createContextDataforClosing(  void *, void *, 
                                                            struct contextDataforClosing *);

#endif // MYLIB_H
