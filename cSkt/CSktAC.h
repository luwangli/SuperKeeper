#ifndef CSKTAC_H
#define CSKTAC_H
//#include "../Sketch/ACEst.h"
class CSktAC{
private:
    // adaptive counting parameters
    ACEst **AC;
    uint32_t seed[10];
    int hash_num;
    float alpha;
    int bkt_num;
    int bkt_size;
    // sketch parameter
    int est_num;
    int ss_depth;
    int ss_width;
    uint32_t *key;
    int *level;
public:
    CSktAC(int mem, int bucket_num, int bucket_size, int depth){
        ss_depth = depth;
        bkt_num = bucket_num;
        bkt_size = bucket_size;
        est_num = mem / (bkt_size*bkt_num + 8*sizeof(int) + 8*sizeof(uint32_t));
        cout<<" est number: "<<est_num<<endl;
        AC = (ACEst **)malloc(sizeof(ACEst **) * est_num);
        for(int i=0;i<est_num;i++){
            AC[i] = newACEst(bkt_num,bkt_size);
        }
        ss_width = est_num / ss_depth;
        for(int i=0;i<depth;i++){seed[i] = i;}

        if(bkt_num ==1 ){
            alpha = 0.77351;
        }else{
            cout<<std::tgamma(-1.0/bkt_num)<<endl;
            cout<<(((1-std::pow(2,1.0/bkt_num))/log(2)))<<endl;
            alpha = std::tgamma(-1.0/bkt_num) * ((( 1-std::pow(2,1.0/bkt_num)) /log(2)));
            alpha = std::pow(alpha,-bkt_num);
            cout<<"alpha is: "<<alpha<<endl;
        }
        //init sketch
        key = (uint32_t *) malloc(sizeof(uint32_t)*est_num);
        level = (int *) malloc(sizeof(int)*est_num);
        for(int i=0;i<est_num;i++){
            key[i] = 0;
            level[i] = 0;
        }
/*
        key = new uint32_t*ss_depth;
        level = new int*ss_depth;
        for(int i=0;i<ss_depth;i++){
            key[i] = new uint32_t[ss_width]();
            level[i] = new int[ss_width]();
        }
        */
    }

    int GetCarCol(uint32_t flowID,int col){
        int index;
     //   int min_R = 1<<30;
        int car;
        float tt=0, EmpBktRat =0;
        index = murmurhash((const char*)&flowID, 4,seed[col]) % ss_width;
        index += col*ss_width;
        if(bkt_num==1){
            tt = ACEstGetCar(AC[index]);
            car = tt / alpha;
            car = tt;
        }else{
            tt = ACEstGetEmptyBuc(AC[index]);
            EmpBktRat = tt*1.0/bkt_num;
            if(EmpBktRat >= 0.051){
                car = -bkt_num*log(tt*1.0/bkt_num);
            }else{
                tt = ACEstGetCar(AC[index]);
                car = alpha*bkt_num*tt;
            }
        }
        return car;
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        int tmp_level;
        for(int i=0;i<ss_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            res += i*ss_width;
            ACEstInsert(AC[res],elemID);
            tmp_level = GetCarCol(flowID,i);
            if(tmp_level >= level[res]){
               // cout<<"have overth?"<<endl;
                key[res] = flowID;
                level[res] = tmp_level;
            }
        }
    }

    void QueryThresh(uint32_t thresh, vector<pair<uint32_t,uint32_t>> &res){
        set<uint32_t> reset;
        for(int i=0;i<ss_depth;i++){
            for(int j=0;j<ss_width;j++){
                if(level[i*ss_width+j] >= (int)thresh){
                    reset.insert(key[i*ss_width+j]);
                }
            }
        }
        for(auto it:reset){
         //   cout<<"it is? "<<it<<endl;
            int val = PointQuery(it);

            if(val >= (int)thresh){
                res.push_back(make_pair(it,val));
            }
        }
    }

    uint32_t PointQuery(uint32_t flowID){
        int val = 1<<20;
        int res;
        for(int i=0;i<ss_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % ss_width;
            res += i*ss_width;
            if(key[res] == flowID){
                val = min(level[res],val);
            }
        }
        return val;
    }
};
#endif
