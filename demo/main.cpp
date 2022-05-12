#include "../common/param.h"
#include "../common/Read.h"
#include "../Estimator/ACEstBit.h"
#include "../Estimator/LCEstBit.h"
#include "../GMF/SktLC.h"
#include "../SpreadSketch/spreadsketch.h"
#include "../cSkt/CSktAC.h"
//#include "../SKAC/superkeepAC.h"
#include "../SKAC/SKPowerAC.h"
#include "../SKLC/SKPowerLC.h"
//#include "../SKLC/superkeepLC.h"
using namespace std;

int main(){
    vector<pair<uint32_t, uint32_t>> Stream;
    vector<pair<uint32_t, uint32_t>> Bench;

    int pkt_num, flow_num;

    char filename[100] = "../data/kaggleData.txt";
    char resname[100] = "../data/kaggleItemSpread.txt";
    pkt_num = Read(filename,Stream);
    flow_num = ReadRes(resname, Bench);
    cout<<"***************************"<<endl;
    timespec start_time, end_time;
    long long timediff=0,det_time=0;
    double insert_throughput=0;
    /*************param settting**********/
    int memory = 100;//KB
    int Th = 500;
    int arraysize = 200;
    int depth = 2;
    float alpha =2;
    float beta =1;
  //  cout<<"Input memory, Threshold and depth, (for example: 100 250 1)"<<endl;
  //  cin >> memory >> Th >> depth;
    /*************result parameter***********/
    double precision=0,recall=0,ab_error=0,re_error=0,f1=0;
    double avg_ab_error=0, avg_re_error=0;
    int tp=0,cnt=0;
    ofstream outFile;

    // *****************************superkeep_power test ************
    cout<<"**************SK-AC *******************"<<endl;
    int skp_size = 4;
    int skp_bkt_num = 40;
    int skp_col_num = depth;
    int skp_mem = memory*1024*8;
    float skp_a = alpha;
    float skp_b = beta;
   // auto tskt = TSktAC(tskt_mem,bkt_num,size,col_num);
    auto skp = SKPowerAC(skp_mem, skp_bkt_num, skp_size, skp_col_num,skp_a,skp_b);
    //insert
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    //pkt_num = 100;
    for(int i=0;i<pkt_num;i++){
        skp.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    vector<pair<uint32_t,uint32_t>> SKPRes;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    skp.QueryThresh(Th, SKPRes);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    det_time = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    det_time = det_time/1000;
    // Calculate accuracy

   // int ab_error=0, re_error=0;
    for(auto it:Bench){
        if(int(it.second) >= Th){
            cnt++;
            for(auto ik:SKPRes){
                if(ik.first == it.first){
                   // cout<<it.first<<"\t real value:"<<it.second<<"\t ss value:"<<ik.second<<endl;
                    ab_error += abs((int)it.second - (int)ik.second);
                   // cout<<"absolute error: "<<ab_error<<endl<<endl;
                    re_error += abs((int)it.second - (int)ik.second) * 1.0 / it.second;
                    tp++;
                }
            }
        }
    }

    cout<<" total superspread: "<<cnt<<"\t est superspread:"<<SKPRes.size()<< "\t detect superspread: "<<tp<<endl;
    precision = tp * 1.0 / SKPRes.size();
    recall = tp*1.0/cnt;
    f1 = 2*precision*recall/(precision+recall);
    avg_ab_error = ab_error/tp;
    avg_re_error = re_error / tp;

    cout<<"Memory \t precision  \t insert_th \t dec_time(us) \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<skp_mem/(8*1024)<<"\t"<<precision<<"\t"<<insert_throughput<<"\t"<<det_time<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;



    cout<<"**************SK-LC *******************"<<endl;

    int skplc_arraysize = arraysize;
    int skplc_colnum = depth;
    int skplc_mem = memory*1024*8;
    float skplc_a = alpha;
    float skplc_b = beta;
   // auto tskt = TSktAC(tskt_mem,bkt_num,size,col_num);
//    auto sk = SuperKeepAC(sk_mem, sk_bkt_num, sk_size, sk_col_num);
    auto skplc = SKPowerLC(skplc_mem,skplc_arraysize,skplc_colnum,skplc_a,skplc_b);
    //insert
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    //pkt_num = 10;
    for(int i=0;i<pkt_num;i++){
        skplc.Insert(Stream[i].second,Stream[i].first);
        //cout<<"insert: "<<i<<endl;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    vector<pair<uint32_t,uint32_t>> SKPLCRes;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    skplc.QueryThresh(Th, SKPLCRes);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    det_time = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    det_time = det_time/1000;
    // Calculate accuracy
    precision=0;recall=0;ab_error=0;re_error=0;f1=0;
    avg_ab_error=0; avg_re_error=0;
    tp=0;cnt=0;
   // int ab_error=0, re_error=0;
    for(auto it:Bench){
        if(int(it.second) >= Th){
            cnt++;
            for(auto ik:SKPLCRes){
                if(ik.first == it.first){
                   // cout<<it.first<<"\t real value:"<<it.second<<"\t ss value:"<<ik.second<<endl;
                    ab_error += abs((int)it.second - (int)ik.second);
                   // cout<<"absolute error: "<<ab_error<<endl<<endl;
                    re_error += abs((int)it.second - (int)ik.second) * 1.0 / it.second;
                    tp++;
                }
            }
        }
    }

    cout<<" total superspread: "<<cnt<<"\t est superspread:"<<SKPLCRes.size()<< "\t detect superspread: "<<tp<<endl;
    precision = tp * 1.0 / SKPLCRes.size();
    recall = tp*1.0/cnt;
    f1 = 2*precision*recall/(precision+recall);
    avg_ab_error = ab_error/tp;
    avg_re_error = re_error / tp;

    cout<<"Memory \t precision  \t insert_th \t dec_time(us) \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<skplc_mem/(8*1024)<<"\t"<<precision<<"\t"<<insert_throughput<<"\t"<<det_time<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    // *********************************cSkt test*********************
    cout<<"**************cSkt*******************"<<endl;
    int size = 4;
    int bkt_num = 40;
    int col_num = depth;
    int cskt_mem = memory*1024*8;
    auto cskt = CSktAC(cskt_mem,bkt_num,size,col_num);
    //insert
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(int i=0;i<pkt_num;i++){
        cskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    vector<pair<uint32_t,uint32_t>> CSKTRes;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    cskt.QueryThresh(Th, CSKTRes);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    det_time = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    det_time = det_time/1000;
    // Calculate accuracy
    precision=0;recall=0;ab_error=0;re_error=0;f1=0;
    avg_ab_error=0; avg_re_error=0;
    tp=0;cnt=0;
   // int ab_error=0, re_error=0;
    for(auto it:Bench){
        if(int(it.second) >= Th){
            cnt++;
            for(auto ik:CSKTRes){
                if(ik.first == it.first){
                   // cout<<it.first<<"\t real value:"<<it.second<<"\t ss value:"<<ik.second<<endl;
                    ab_error += abs((int)it.second - (int)ik.second);
                   // cout<<"absolute error: "<<ab_error<<endl<<endl;
                    re_error += abs((int)it.second - (int)ik.second) * 1.0 / it.second;
                    tp++;
                }
            }
        }
    }

    cout<<" total superspread: "<<cnt<<"\t est superspread:"<<CSKTRes.size()<< "\t detect superspread: "<<tp<<endl;
    precision = tp * 1.0 / CSKTRes.size();
    recall = tp*1.0/cnt;
    f1 = 2*precision*recall/(precision+recall);
    avg_ab_error = ab_error/tp;
    avg_re_error = re_error / tp;

    cout<<"Memory \t precision  \t insert_th \t dec_time(us) \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<cskt_mem/(8*1024)<<"\t"<<precision<<"\t"<<insert_throughput<<"\t"<<det_time<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    // *******************************SpreadSketch teset************************
    // Spread Sketch setting
     cout<<"**************SpreadSketch*******************"<<endl;
    int lgn = 32;
    int cmdepth ;
    int cmwidth ;
    cmdepth = depth;
    //cmwidth = 1500;
    int b = 79;
    int c = 3;
    int s_memory = 438;
    cmwidth = (memory*1024*8)/((s_memory+lgn+8)*cmdepth);
    cout<<"est num: "<<cmdepth*cmwidth<<endl;
    int total_mem = cmdepth*cmwidth*(s_memory+lgn+8)/1024/8;
    //auto AC = SktAD(memory);
    DetectorSS *sketch = new DetectorSS(cmdepth, cmwidth, lgn, b, c, s_memory);

    // ***** SS data insert
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(int i=0;i<pkt_num;i++){
        sketch->Update(Stream[i].second,Stream[i].first,1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;


    //SS get superspread result
    vector<pair<uint32_t,uint32_t>> SSRes;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    sketch->Query(Th,SSRes);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    det_time = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    det_time = det_time/1000;
    // Calculate accuracy
    precision=0;recall=0;ab_error=0;re_error=0;f1=0;
    avg_ab_error=0; avg_re_error=0;
    tp=0;cnt=0;
   // int ab_error=0, re_error=0;
    for(auto it:Bench){
        if(int(it.second) >= Th){
            cnt++;
            for(auto ik:SSRes){
                if(ik.first == it.first){
                   // cout<<it.first<<"\t real value:"<<it.second<<"\t ss value:"<<ik.second<<endl;
                    ab_error += abs((int)it.second - (int)ik.second);
                   // cout<<"absolute error: "<<ab_error<<endl<<endl;
                    re_error += abs((int)it.second - (int)ik.second) * 1.0 / it.second;
                    tp++;
                }
            }
        }
    }
    cout<<" total superspread: "<<cnt<<"\t est superspread:"<<SSRes.size()<< "\t detect superspread: "<<tp<<endl;
    precision = tp * 1.0 / SSRes.size();
    recall = tp*1.0/cnt;
    f1 = 2*precision*recall/(precision+recall);
    avg_ab_error = ab_error/tp;
    avg_re_error = re_error / tp;

    cout<<"Memory \t precision  \t insert_th \t dec_time(us) \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<total_mem<<"\t"<<precision<<"\t"<<insert_throughput<<"\t"<<det_time<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;


    // ***************************************GMF test****************************
    // GMF setting
     cout<<"**************GMF*******************"<<endl;

    int gmf_arragysize=arraysize;
    int gmf_depth = depth;
    int gmf_mem = memory;
//    auto tB = SktAC(gmf_mem*1024*8,bkt_num,size,col_num);
    auto tB = SktLC(gmf_mem*1024*8,gmf_arragysize,gmf_depth);
    // ***** GMF data insert
    vector<pair<uint32_t,uint32_t>> GMFRes;
    int gmf_count = 0;
    //set<uint32_t> FL;
    bool gmf_flag = false;
    int gmf_res;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(int i=0;i<pkt_num;i++){
        gmf_flag = false;
        for(auto it:GMFRes){
            if(it.first == Stream[i].second){
                gmf_flag = true;
                break;
            }
        }
        if(!gmf_flag){
            tB.Insert(Stream[i].second, Stream[i].first);
            gmf_res = tB.GetCar(Stream[i].second);
            if(gmf_res>=Th && gmf_count <=1000){
                GMFRes.push_back(make_pair(Stream[i].second,gmf_res));
                gmf_count++;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    //SS get superspread result
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    //sketch->Query(Th,SSRes); a filter design
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    det_time = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    det_time = det_time/1000;
    // Calculate accuracy
    precision=0;recall=0;ab_error=0;re_error=0;
    avg_ab_error=0;avg_re_error=0;
    tp=0,cnt=0;
   // int ab_error=0, re_error=0;
    for(auto it:Bench){
        if(int(it.second) >= Th){
            cnt++;
            for(auto ik:GMFRes){
                if(ik.first == it.first){
                  //  cout<<it.first<<"\t real value:"<<it.second<<"\t ss value:"<<ik.second<<endl;
                    ab_error += abs((int)it.second - (int)ik.second);
                   // cout<<"absolute error: "<<ab_error<<endl<<endl;
                    re_error += abs((int)it.second - (int)ik.second) * 1.0 / it.second;
                    tp++;
                }
            }
        }
    }
    cout<<" total superspread: "<<cnt<<"\t est superspread:"<<GMFRes.size()<< "\t detect superspread: "<<tp<<endl;
    precision = tp * 1.0 / GMFRes.size();
    recall = tp*1.0/cnt;
    f1 = 2*precision*recall/(precision+recall);
    avg_ab_error = ab_error/tp;
    avg_re_error = re_error / tp;
   // cout<<"**************GMF*******************"<<endl;
    cout<<"Memory \t precision \t insert_th \t dec_time(us) \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<gmf_mem<<"\t"<<precision<<"\t"<<insert_throughput<<"\t"<<det_time<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;



}
