#ifndef PARAM_H
#define PARAM_H

#include <iostream>
#include <unordered_map>
#include <memory.h>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <math.h>
#include <cmath>
#include "./hash.h"
//#include <linux/slab.h>

#define MAX_INSERT_PACKET 32000000

int NumberOfLeadingZeros(uint32_t x){
    int n=0;
    if(x <= 0x0000ffff){n+=16;x<<=16;}
    if(x <= 0x00ffffff){n+=8;x<<=8;}
    if(x <= 0x0fffffff){n+=4;x<<=4;}
    if(x <= 0x3fffffff){n+=2,x<<=2;}
    if(x <= 0x7fffffff) n++;
    return n;
}

#endif
