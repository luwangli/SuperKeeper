#ifndef SKTAC_H
#define SKTAC_H
//#include "../common/hash.h"
//#include "./ACEst.h"

class SktAC{
private:
    ACEst **AC;
    uint32_t seed[10];
  //  int hash_num;
    int est_num;
    float alpha;
    int bkt_num;
    int col_num;
    int col_size;
public:
    SktAC(int mem,int bucket_num, int size, int col){
    //    hash_num = h_num;
        col_num = col;
        bkt_num = bucket_num;
        est_num = mem /(bkt_num * size);
        cout << "est num: "<<est_num<<endl;
        AC = (ACEst **) malloc(sizeof(ACEst **) * est_num);
        for(int i=0;i<est_num;i++){
            AC[i] = newACEst(bkt_num,size);
        }
        col_size = est_num / col_num;
        for(int i=0;i<col_num;i++){seed[i] = i;}
        alpha = std::tgamma(-1.0/bkt_num) * ((( 1-std::pow(2,1.0/bkt_num)) /log(2)));
        alpha = std::pow(alpha,-bkt_num);
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        for(int i =0;i<col_num;i++){
            //hierarchy process
            res = murmurhash((const char*)&flowID,4,seed[i]) % col_size;
            res += i*col_size;
            ACEstInsert(AC[res],elemID);
        }
    }
    int GetCar(uint32_t flowID){
        int index;
        int min_R = 1<<30;
        int car;
        float tt = 0;
        float EmpBktRat = 0;
        for(int i=0;i<col_num;i++){
            index = murmurhash((const char*)&flowID,4,seed[i]) % col_size;
            index += i*col_size;
            tt = ACEstGetEmptyBuc(AC[index]);
            EmpBktRat = tt*1.0/bkt_num;
            if(EmpBktRat >= 0.051){
          //      cout<<"empty bucket ration over 0.051, use LC"<<endl;
                car = -bkt_num * log(tt*1.0/bkt_num);
            }else{
            //    cout<<"empty bucket ration less 0.051, use LL"<<endl;
                tt = ACEstGetCar(AC[index]);
                car = alpha * bkt_num *tt;
            }
            min_R = min(car,min_R);
        }
        car = min_R;
        return car;
    }
};
#endif
