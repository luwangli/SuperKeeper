#ifndef LCEST_H
#define LCEST_H

#include "../common/param.h"

typedef struct{
    int seed;
    int LCSize;
    int maxRegisterValue;
   // bool *LC;
   char *LC;
   int num;
}LCEst;

LCEst *newLCEst(int size){
    LCEst *h = (LCEst*)malloc(sizeof(LCEst));
    h->LCSize = size;
    h->seed = 150;
    h->num = size/8+1;
    //ceil(size/8);
    h->LC = (char*)malloc(sizeof(char) * h->num);
    for(int i=0;i<(h->num );i++){
        h->LC[i] = 0;
    }
    /*
    h->LC = (bool*)malloc(sizeof(bool) * h->LCSize);

    for(int i=0;i<h->LCSize;i++){
        h->LC[i] = 0;
    }*/
    h->maxRegisterValue = size;
    return h;
}

void ClearOneEstLC(LCEst *h){
/*
    for(int i=0;i<h->LCSize;i++){
        h->LC[i] = 0;
    }
    */
//    h->LC = {0};

    memset(h->LC,0,sizeof(char)*(h->num));
}

void LCEstInsert(LCEst *h, uint32_t elemID){
    uint32_t hash_val =  murmurhash((const char*)&elemID,4,h->seed);

    int bktInd = hash_val % h->LCSize;

    //h->LC[bktInd] = true;
    h->LC[bktInd>>3] |= (0x01<<((bktInd)&0x07));
    //cout<<"after insert: "<<h->LC[bktInd>>3]<<endl;

}

int LCEstGetCar(LCEst *h){

    /*
    int res = 0;
    for(int i=0;i<h->LCSize;i++){
        if(!h->LC[i]) res++;
    }
    return res;
*/
    int res = 0;
//    /char a = 0x01;
    char cc ;
    /*
    for(int i=0;i<ceil(h->LCSize/8);i++){
       // cout<<"i is "<<i<<endl;
        cc = h->LC[i];
        while(cc!=0){
            if((cc&0x80) == 1)
                res++;
            cc <<= 1;
        }
    }*/
    /*
    cout<<" a new *******************"<<endl;
    for(int i=0;i<ceil(h->LCSize/8);i++){
        cc = h->LC[i];
        cout<<"cc is: "<<cc<<endl;
        for(int j=0;j<8;j++){
            if((cc & (0x01<<j)) == 1) res++;

        }
        cout<<"res: "<<res<<endl;
    }*/
    // method 1
    /*
    for(int i=0;i<h->num;i++){

        for(int j=0;j<8;j++){
            res+= h->LC[i]>>j & 1;
        }
    }*/
    //method 2
    for(int i =0;i<h->num;i++){
        cc = h->LC[i];
        cc = (cc & 0x55) + ((cc >> 1) & 0x55);
        cc = (cc & 0x33) + ((cc >> 2) & 0x33);
        cc = (cc & 0x0F) + ((cc >> 4) & 0x0F);
        res += (int)cc;
    }

    res = h->LCSize - res;
   // cout<<"res: "<<res<<endl;
    return res;

}

#endif
