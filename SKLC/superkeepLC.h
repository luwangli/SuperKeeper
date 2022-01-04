#ifndef SUPERKEEPLC_H
#define SUPERKEEPLC_H
//#include "../Estimator/LCEst.h"
class SuperKeepLC{
private:
    // LC estimator
    LCEst **PLC;
    LCEst **NLC;
    uint32_t seed[10];
    int arraySize;
    //sketch parameter
    int est_num;
    int ss_depth;
    int ss_width;
    uint32_t *key;
    int *pvote;
    int *nvote;
   // float pw_a = 1;
   // float pw_b = 1;
public:
    SuperKeepLC(int mem, int size, int depth){
        // init estimator
        ss_depth = depth;
        arraySize = size;
        est_num = mem /(2*arraySize + 8*sizeof(int)*2 + 8*sizeof(uint32_t));
//        est_num = mem / (2*bkt_size*bkt_num + 8*sizeof(int) + 8*sizeof(int) + 8*sizeof(uint32_t));
        cout<<"est num: "<<est_num<<endl;
        PLC = (LCEst **)malloc(sizeof(LCEst **) * est_num);
        NLC = (LCEst **)malloc(sizeof(LCEst **) * est_num);
        for(int i=0;i<est_num;i++){
            PLC[i] = newLCEst(arraySize);
            NLC[i] = newLCEst(arraySize);
        }
        ss_width = est_num / ss_depth;
        for(int i=0;i<ss_depth;i++){seed[i] = i*3;}

        // init sketch
      //  pw_a = a;
     //   pw_b = b;
        key = (uint32_t *)malloc(sizeof(uint32_t)*est_num);
        pvote = (int*)malloc(sizeof(int)*est_num);
        nvote = (int*)malloc(sizeof(int)*est_num);
        for(int i=0;i<est_num;i++){
            key[i] =0;
            pvote[i] = 0;
            nvote[i] = 0;
        }
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        for(int i=0;i<ss_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            res += i*ss_width;
            if(key[res] == 0){
                key[res] = flowID;
                LCEstInsert(PLC[res],elemID);
                pvote[res] = 1;
//                pvote[res] = UpdateVote(PLC[res]);
               // cout<<"choise 0, res: "<<pvote[res]<<endl;
            } else{
                if(key[res] == flowID){
                    //increment pvote
                    LCEstInsert(PLC[res],elemID);
                    pvote[res] = UpdateVote(PLC[res]);
                   // cout<<"choise 1, res: "<<pvote[res]<<endl;
                }else{
                    //increment nvote with a probability
                    LCEstInsert(NLC[res],elemID);
                    nvote[res] = UpdateVote(NLC[res]);
                      //  cout<<"choise 2, res: "<<nvote[res]<<endl;
                    if(nvote[res] >= pvote[res]){
                        Clear(res);
                    }
                }
            }
        }
    }

    void QueryThresh(uint32_t thresh, vector<pair<uint32_t,uint32_t>> &res){


        set<uint32_t> reset;
        for(int i=0;i<ss_depth;i++){
            for(int j=0;j<ss_width;j++){
                if(pvote[i*ss_width+j] >= (int)thresh){
                    reset.insert(key[i*ss_width+j]);
                }
            }
        }
        for(auto it:reset){
            int val = PointQuery(it);
            if(val >=(int)thresh){
                res.push_back(make_pair(it,val));
            }
        }
    }

    uint32_t PointQuery(uint32_t flowID){
        int val = 1<<25;
        int res;
        bool flag = false;
        for(int i=0;i<ss_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            res += i*ss_width;
            if(key[res] == flowID){
                val = min(pvote[res],val);
                flag = true;
            }
        }
        if(flag){return val;}
        else{ return 0;}
    }

    void Clear(int pos){
        //clear a cell
        pvote[pos] = 0;
        nvote[pos] = 0;
        key[pos] = 0;
        ClearOneEstLC(PLC[pos]);
        ClearOneEstLC(NLC[pos]);
    }

    int UpdateVote(LCEst *h){
        int car = LCEstGetCar(h);
        int res = -arraySize*log(car*1.0/arraySize);
        return res;
    }

/*
    int UpdateVote(LCEst *h){
        int car;

        float EmpBkt = LCEstGetEmptyBuc(h);
        float EmpBktRat = EmpBkt*1.0/bkt_num;
        if(EmpBktRat >= 0.051){
            //LC counting
            car = -bkt_num * log(EmpBkt*1.0/bkt_num);
        }else{
            //LL counting
            float tt = LCEstGetCar(h);
            car = alpha * bkt_num * tt;
        }
        return car;
    }*/

    int GetCar(uint32_t flowID){
        int min_R = 1<<25;
        int car;
        int index;
        for(int i=0;i<ss_depth;i++){
            index = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            index += ss_width*i;
            car = pvote[index];
            min_R = min(car,min_R);
        }
        car = min_R;
        return car;
    }
};
#endif
