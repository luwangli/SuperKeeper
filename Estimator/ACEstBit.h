#ifndef ACEST_H
#define ACEST_H
#include "../common/param.h"

typedef struct{
    int seed;
    int ACSize;
    int BucketNum;
    int maxRegisterValue;
    char *AC;
    int charnum;
}ACEst;

void setBitsSetValueAC(const ACEst *h, int index, int value){
    /*
    int i =0;
    while(value !=0 && i <h->ACSize){
        if((value&1)!=0){h->AC[index][i] = 1;}
        else h->AC[index][i] = 0;
        value = value >>1;
        i++;
    }*/
    h->AC[index] = (char)(value&0x000000ff);
}

int getBitsSetValueAC(const ACEst *h,int b){
    int res =0;
    /*
    char cc = h->AC[b];
    cc = (cc & 0x55) + ((cc >> 1) & 0x55);
    cc = (cc & 0x33) + ((cc >> 2) & 0x33);
    cc = (cc & 0x0F) + ((cc >> 4) & 0x0F);
    res += (int)cc;
    */
    res = (int)(h->AC[b]);
    return res;
    /*
    int res = 0;
    for(int i=0;i<h->ACSize;i++){
        if(b[i]){
            res += (1<<i);
        }
    }
    return res;*/
}

ACEst *newACEst(int m, int size){
    ACEst *h = (ACEst*)malloc(sizeof(ACEst));
    h->ACSize = size;
    h->BucketNum = m;
    h->seed = 101;
    h->charnum = size/8+1;// always 1;
   // h->AC = (bool **)malloc(sizeof(bool*) * h->BucketNum);
    h->AC = (char*)malloc(sizeof(char) * h->BucketNum);
    for(int i=0;i<h->BucketNum;i++){
        //h->AC[i] = (bool*)malloc(8*sizeof(bool));
       // h->AC[i] = (char)malloc(sizeof(char));
        h->AC[i] = 0;

    }
    h->maxRegisterValue = std::pow(2,size)-1;
   // h->maxRegisterValue = (int)(31);
    return h;
}

void ClearOneEst(ACEst *h){
    for(int i=0;i<h->BucketNum;i++){
        //memset(h->AC[i],0,sizeof(bool)*h->ACSize);
        h->AC[i] =0;
    }
}

void ACEstInsert(ACEst *h, uint32_t elemID){
//    uint32_t hash_val = h->Esthash->run((const char*)&elemID,KEY_LEN);
    uint32_t hash_val =  murmurhash((const char*)&elemID,4,h->seed);

    int bktInd = hash_val % h->BucketNum;
  //  cout<<"elemID: "<<elemID<<"\t bucketIndex: "<<bktInd<<endl;
    hash_val = hash_val | 0x000007f;
    int fstOnePos = NumberOfLeadingZeros(hash_val) + 1;
    fstOnePos = min(h->maxRegisterValue,fstOnePos);
    if(getBitsSetValueAC(h, bktInd) < fstOnePos){
        setBitsSetValueAC(h,bktInd, fstOnePos);
     //   cout<<"count replace: "<<fstOnePos<<endl;
    }
    else{
   //  cout<<"count keep: "<<getBitsSetValueAC(h->AC[bktInd])<<endl;
    }
}

int ACEstGetEmptyBuc(ACEst *h){
    int res =0;
    int maxT;
    for(int i =0;i<h->BucketNum;i++){
        maxT = getBitsSetValueAC(h, i);
        if(maxT==0) res++;
    }
    return res;
}

float ACEstGetCar(ACEst *h){
    float res = 0;
    int maxT;
    for(int i=0;i<h->BucketNum;i++){
        maxT = getBitsSetValueAC(h, i);
      //  cout<<"value in bucket "<<i<<" "<<maxT<<endl;
        res = res + maxT;
    }
    res = res / h->BucketNum;
    res = std::pow(2,res);
    return res;
}

#endif
