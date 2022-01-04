#include "../common/param.h"
#include "../common/Read.h"
#include "../Estimator/LCEstBit.h"

#include "./SKPowerLC.h"
//#include "../common/Murmurhash2.h"
using namespace std;

int main(){
    vector<pair<uint32_t, uint32_t>> Stream;
    vector<pair<uint32_t, uint32_t>> Bench;

    int pkt_num, flow_num;
//    char filename[100] = "../../data/datatrace.txt";
    char filename[100] = "../data/kaggleData.txt";

    /*
    char filename[100];
    cout<<"Input detect file (for example: ../data/datatrace.txt) "<<endl;
    cin >> filename;
*/
//    char resname[100] = "../../data/dstspread.txt";
    char resname[100] = "../data/kaggleItemSpread.txt";

    pkt_num = Read(filename,Stream);
    flow_num = ReadRes(resname, Bench);
    cout<<"***************************"<<endl;
    timespec start_time, end_time;
    long long timediff=0,det_time=0;
    double insert_throughput=0,query_throughput=0;
    /*************param settting**********/
    int memory = 2000;//KB
    int Th = 150;

    // *****************************superkeep_power test ************
    // ******************************superkeep test******************
    cout<<"**************SKP-LC *******************"<<endl;
    int skp_col_num = 2;
    int skp_arraysize = 200;
    int skp_mem = 320000;
    float skp_a = 1;
    float skp_b = 1;

    cout<<"Input , memory, arraysize, depth alpha, beta"<<endl;
    cout<<"for example: 40  200 2 1.0 1.0"<<endl;
    cin>>skp_mem>>skp_arraysize>>skp_col_num>>skp_a>>skp_b;
    skp_mem = skp_mem*1024*8;//input KB;
   // auto tskt = TSktAC(tskt_mem,bkt_n  um,size,col_num);
    auto skp = SKPowerLC(skp_mem, skp_arraysize, skp_col_num,skp_a,skp_b);
    //insert
    clock_gettime(CLOCK_MONOTONIC, &start_time);
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
    det_time = det_time/1000;//ns
    // Calculate accuracy
    double precision=0,recall=0,ab_error=0,re_error=0,f1=0;
    double avg_ab_error=0, avg_re_error=0;
    int tp=0,cnt=0;
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

    cout<<" total superspread: "<<cnt<< "\t detect superspread: "<<tp<<endl;
    precision = tp * 1.0 / SKPRes.size();
    recall = tp*1.0/cnt;
    f1 = 2*precision*recall/(precision+recall);
    avg_ab_error = ab_error/tp;
    avg_re_error = re_error / tp;

    cout<<"Memory \t precision \t recall \t f1 \t insert_th \t dec_time(ms) \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<skp_mem/(8*1024)<<"\t"<<precision<<"\t"<<recall<<"\t"<<f1<<"\t"<<insert_throughput<<"\t"<<det_time<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;
    ofstream outFile;
    outFile.open("result.csv",ios::app);
    outFile <<filename<<",skplc,"<<skp_mem/(8*1024)<<","<<skp_arraysize<<","<<skp_col_num<<","<<skp_a<<","<<skp_b<<","<<
            precision<<","<<recall<<","<<f1<<","<<insert_throughput<<","<<det_time<<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

}
