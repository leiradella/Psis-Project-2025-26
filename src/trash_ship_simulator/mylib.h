#ifndef MYLIB_H

#define MYLIB_H

struct contextDataforClosing{
    void (*pFunction)(void *);
    void *pArguments;
    struct contextDataforClosing *pPreviousStruct;
};

void closeContexts(struct contextDataforClosing *);

struct contextDataforClosing *createContextDataforClosing(  void *, void *, 
                                                            struct contextDataforClosing *);

#endif // MYLIB_H
