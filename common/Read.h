// read data stream, return cardinality

#ifndef _READ_H
#define _READ_H
#include "param.h"
using namespace std;
#define MAX_FLOW 1000000
#define MAX_PACKET 32000000

struct RetVal{
    int pkt_num;
    vector<pair<uint32_t,uint32_t>> Stream;
    unordered_map<uint32_t,int> Bench;
};

int Read(const char *filename, vector<pair<uint32_t,uint32_t>> &Datastream)
{
    FILE *Fin = fopen(filename,"rb");
    if(!Fin){
        cerr << "cannot open file: "<<filename<<endl;
        exit(-1);
    }

    ifstream fin(filename);
    string line;
    uint32_t srcIP, dstIP;
 //   vector<pair<uint32_t,uint32_t>> Datastream;
    int count =0;
    while (getline(fin,line)){
        stringstream ss(line);
        ss>>srcIP>>dstIP;
      //  cout<<srcIP<<"\t"<<dstIP<<endl;
        Datastream.push_back(make_pair(srcIP,dstIP));
        count++;
        if(count == MAX_PACKET){
            cout<<"reach max packet, please adjust MAX_PACKET"<<endl;
        }
    }
    fin.clear();
    fin.close();
    cout<<"Successfully read "<<filename <<endl;
    cout<<"*********data stream info***********"<<endl;
    cout<<"packet num: "<<count<<endl;
    return count;
}

int ReadRes(const char *filename, vector<pair<uint32_t,uint32_t>> &Bench){
    FILE *Fin = fopen(filename,"rb");
    if(!Fin){
        cerr << "cannot open file: "<<filename<<endl;
        exit(-1);
    }

    ifstream fin(filename);
    string line;
    uint32_t flowID, num;
    int count =0;
    while(getline(fin,line)){
        stringstream ss(line);
        ss>>flowID>>num;
        Bench.push_back(make_pair(flowID,num));
        count++;
        if(count == MAX_FLOW){
            cout<<"reach max flow, please adjust MAX_FLOW"<<endl;
        }
    }
    fin.close();
    fin.clear();
    cout<<"successfully read "<<filename <<endl;
    cout<<"**********stream real result*******"<<endl;
    cout<<"flow num: "<<count<<endl;
    return count;
}
RetVal TraceRead(char const *filename)
{
    cout<<"what"<<endl;
    vector<pair<uint32_t,uint32_t>> Datastream;
    unordered_map<uint32_t,int> Bench;
    FILE *Fin = fopen(filename,"rb");
    if(!Fin){
        cerr << "cannot open file: "<<filename<<endl;
        exit(-1);
    }

    ifstream fin(filename);
    string line;
    uint32_t srcIP, dstIP;
    vector<uint32_t> Flow;
    set<uint32_t> Spread[MAX_FLOW];
    int count =0;
    int pos;
    while (getline(fin,line)){
        stringstream ss(line);
        ss>>srcIP>>dstIP;
        Datastream.push_back(make_pair(srcIP,dstIP));
        auto Flag = find(Flow.begin(),Flow.end(),srcIP);
        if (Flag != Flow.end()){
            //find srcIP
            pos = std::distance(Flow.begin(),Flag);
            Spread[pos].insert(dstIP);
        }else{
            Flow.push_back(srcIP);
            pos = Flow.size() - 1;
            Spread[pos].insert(dstIP);
            if(pos==MAX_FLOW){
                cout<<"reach max flow number, please adjust MAX_FLOW"<<endl;
                break;
            }
        }
        count++;
        if(count == MAX_PACKET){
            cout<< "reach max packet number, please adjust MAX_PACKET"<<endl;
            break;
        }
    }
    int t=0;
    for(auto it=Flow.begin();it!=Flow.end();it++){
        Bench.insert(make_pair(*it,Spread[t].size()));
        t++;
    }
    fin.clear();
    fin.close();
    cout<<"Successfully read "<<filename <<endl;
    cout<<"*********data stream info***********"<<endl;
    cout<<"flow number: "<<t<<"\t packet num: "<<count<<endl;
    return {count,Datastream,Bench};
}

#endif
