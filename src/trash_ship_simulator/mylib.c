#include <stdio.h>
#include "mylib.h"

void closeContexts(struct contextDataforClosing *Data){
    printf("Executing close contexts\n");
    (Data->pFunction)((void*)Data->pArguments);
    printf("Completed close contexts\n");
    printf("%p\n", Data->pPreviousStruct);
    if((Data->pPreviousStruct)){
        printf("Recurisng\n");
        closeContexts(Data->pPreviousStruct);
    }
    free(Data);
}

struct contextDataforClosing *createContextDataforClosing(  void *pInFunction, void *pInArguments, 
                                                        struct contextDataforClosing *pInPreviousStruct){

    struct contextDataforClosing *newStruct = malloc(sizeof(struct contextDataforClosing));

    if(!newStruct){
        closeContexts(pInPreviousStruct);
        printf("Failed to create programContext(malloc).\n");
        return NULL;
    }

    newStruct->pFunction = pInFunction;
    newStruct->pArguments = pInArguments;
    newStruct->pPreviousStruct = pInPreviousStruct;
    return newStruct;
}