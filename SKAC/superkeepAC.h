#ifndef SUPERKEEPAC_H
#define SUPERKEEPAC_H
class SuperKeepAC{
private:
    //pvote;
    ACEst **PAC;
    //nvote
    ACEst **NAC;

    uint32_t seed[10];
    float alpha;
    int bkt_num;
    int bkt_size;
    //sketch parameter
    int est_num;
    int ss_depth;
    int ss_width;
    uint32_t *key;
    int *pvote;
    int *nvote;
public:
    SuperKeepAC(int mem, int bucket_num, int bucket_size, int depth){
        // init estimator
        ss_depth = depth;
        bkt_num = bucket_num;
        bkt_size = bucket_size;
        est_num = mem / (2*bkt_size*bkt_num + 8*sizeof(int) + 8*sizeof(int) + 8*sizeof(uint32_t));
        cout<<"est num: "<<est_num<<endl;
        PAC = (ACEst **)malloc(sizeof(ACEst **) * est_num);
        NAC = (ACEst **)malloc(sizeof(ACEst **) * est_num);
        for(int i=0;i<est_num;i++){
            PAC[i] = newACEst(bkt_num, bkt_size);
            NAC[i] = newACEst(bkt_num, bkt_size);
        }
        ss_width = est_num / ss_depth;
        for(int i=0;i<ss_depth;i++){seed[i] = i*3;}
        // define alpha
        alpha = std::tgamma(-1.0/bkt_num) * ((( 1-std::pow(2,1.0/bkt_num)) /log(2)));
        alpha = std::pow(alpha,-bkt_num);
        cout<<"alpha is: "<<alpha<<endl;
        // init sketch
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
                ACEstInsert(PAC[res],elemID);
                pvote[res] = UpdateVote(PAC[res]);
            } else{
                if(key[res] == flowID){
                    //increment pvote
                    ACEstInsert(PAC[res],elemID);
                    pvote[res] = UpdateVote(PAC[res]);
                }else{
                    //increment nvote
                    ACEstInsert(NAC[res],elemID);
                    nvote[res] = UpdateVote(NAC[res]);
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
        ClearOneEst(PAC[pos]);
        ClearOneEst(NAC[pos]);
    }

    int UpdateVote(ACEst *h){
        int car;

        float EmpBkt = ACEstGetEmptyBuc(h);
        float EmpBktRat = EmpBkt*1.0/bkt_num;
        if(EmpBktRat >= 0.051){
            //LC counting
            car = -bkt_num * log(EmpBkt*1.0/bkt_num);
        }else{
            //LL counting
            float tt = ACEstGetCar(h);
            car = alpha * bkt_num * tt;
        }
        return car;
    }

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
