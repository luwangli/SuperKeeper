#ifndef LCEST_H
#define LCEST_H

#include "../common/param.h"

typedef struct{
    int seed;
    int LCSize;
    int maxRegisterValue;
    bool *LC;
}LCEst;

LCEst *newLCEst(int size){
    LCEst *h = (LCEst*)malloc(sizeof(LCEst));
    h->LCSize = size;
    h->seed = 150;
    h->LC = (bool*)malloc(sizeof(bool) * h->LCSize);
    for(int i=0;i<h->LCSize;i++){
        h->LC[i] = 0;
    }
    h->maxRegisterValue = size;
    return h;
}

void ClearOneEstLC(LCEst *h){

    memset(h->LC,0,sizeof(bool)*h->LCSize);
}

void LCEstInsert(LCEst *h, uint32_t elemID){
    uint32_t hash_val =  murmurhash((const char*)&elemID,4,h->seed);
    int bktInd = hash_val % h->LCSize;
    h->LC[bktInd] = true;
}

int LCEstGetCar(LCEst *h){

    int res = 0;
    for(int i=0;i<h->LCSize;i++){
        if(!h->LC[i]) res++;
    }
    //cout<<"res: "<<res<<endl;
    return res;
}
#endif
