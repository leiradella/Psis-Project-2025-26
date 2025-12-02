#ifndef MYLIB_H

#define MYLIB_H

#define MYLEFT  8
#define MYUP    4
#define MYDOWN  2
#define MYRIGHT 1

struct contextDataforClosing{
    void (*pFunction)(void *);
    void *pArguments;
    struct contextDataforClosing *pPreviousStruct;
};

void closeContexts(struct contextDataforClosing *);

struct contextDataforClosing *createContextDataforClosing(  void *, void *, 
                                                            struct contextDataforClosing *);

#endif // MYLIB_H
