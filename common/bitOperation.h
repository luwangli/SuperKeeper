/*thies file contains operations about bit which is
widely used in cardinality estiamtion*/

#ifndef _BITMAP_H
#define _BITMAP_H

//get the bit in n position
bool GetBit( char* bitmap, int n){
    return ((bitmap[n>>3]) & (0x01 << (n&0x07)));
}

//set the bit to 1
void SetBit(char* bitmap, int n){
    bitmap[n>>3]  |= (0x01<<(n&0x07));
}

//set the bit to 0
void ClearBit(char* bitmap, int n){
    bitmap[n>>3] &= (~(0x01<<(n&0x07)));
}

//initialize all bit to zero
//void FillZero(char *bitmap){    }

//int GetBitmapSize(){}

int CountZeroBits(char *bitmap, int start, int end){
    int res =0;
    for(int i = start;i<end;i++){
        if(!GetBit(bitmap,i)){res++;}
    }
    return res;
}

#endif
