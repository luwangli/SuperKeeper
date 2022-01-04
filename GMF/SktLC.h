#ifndef SKTLC_H
#define SKTLC_H

class SktLC{
private:
    LCEst **LC;
    uint32_t seed[10];
    int est_num;
    int arraySize;
    int ss_depth;
    int ss_width;
public:
    SktLC(int mem, int size, int depth){
        ss_depth = depth;
        arraySize = size;
        est_num = mem / arraySize;
        cout<<"est num: "<<est_num<<endl;
        LC = (LCEst **) malloc(sizeof(LCEst**)*est_num);
        for(int i=0;i<est_num;i++){
            LC[i] = newLCEst(arraySize);
        }
        ss_width = est_num /ss_depth;
        for(int i=0;i<ss_depth;i++){seed[i] = i*3;}
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        for(int i=0;i<ss_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            res += i*ss_width;
            LCEstInsert(LC[res],elemID);
        }
    }

    int GetCar(uint32_t flowID){
        int index;
        int car;
        int min_R = 1<<25;
        for(int i=0;i<ss_depth;i++){
            index = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            index += i*ss_width;
            car = LCEstGetCar(LC[index]);
            min_R = min(car,min_R);
        }
        car = min_R;
        car = -arraySize*log(car*1.0/arraySize);
        return car;
    }
};
#endif
