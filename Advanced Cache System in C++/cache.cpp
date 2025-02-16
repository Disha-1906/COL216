#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <math.h>
#include <cstring>
#include <deque>
#include <limits>

using namespace std;

#define NotApplicable 0 
#define ReadHit 1
#define ReadMiss 2
#define WriteHit 3
#define WriteMiss 4

struct Mem_Hierarchy{
    int L1blocksize;
    int L1assoc;
    int L1size;
    int L2blocksize;
    int L2assoc;
    int L2size;
};

int hex_to_int(string input_addr){
    stringstream ss;
    ss << hex << input_addr;
    unsigned int intVal;
    ss >> intVal;
    return intVal;
};

float inf = numeric_limits<float>::infinity();

struct Block {
    unsigned long data;
    unsigned long tag;
    unsigned long offset;
    bool valid_bit = 0;
    bool dirty_bit;
    int lru_index = 0;
};

struct Cache {
    Block blk;
    int nSets;
    bool same_tag;
    bool readMiss;
    unsigned long counter_col;
    unsigned long index;

    int total_reads;
    int read_miss;
    int miss_rate;
    int total_writes;
    int write_miss;
    int writeback;
    int access_state;

    void set_nSets( int blockSize, int cacheSize, int assoc){
        nSets = cacheSize/(assoc*blockSize);
    }

    void compute(int addr, int blockSize, int cacheSize, int assoc){
    blk.offset = addr %  blockSize;
    int new_addr = addr/blockSize;
    int setSize = cacheSize/(assoc*blockSize);
    index = new_addr % setSize;
    blk.tag = new_addr / setSize;
    }
};

int execute( vector<int> arra,string filename){
    unsigned long count_1 = -1, count_2 = -1;
    Cache L1,L2;
    L1.counter_col = 0;
    L2.counter_col = 0;
    Mem_Hierarchy Memory;
    // ifstream cache_params;
    // string pseudoline;
    // cache_params.open("cacheconfig.txt");
        // cache_params>>pseudoline;
        Memory.L1blocksize = arra.at(0);
        Memory.L1assoc = arra.at(2);
        Memory.L1size = arra.at(1);
        // cache_params>>pseudoline; 
        Memory.L2blocksize = arra.at(0);
        Memory.L2assoc = arra.at(4);
        Memory.L2size = arra.at(3);   

    L1.set_nSets(Memory.L1blocksize,Memory.L1size,Memory.L1assoc);
    L2.set_nSets(Memory.L2blocksize,Memory.L2size,Memory.L2assoc);

    if(Memory.L1assoc == 0){
        Memory.L1assoc=L1.nSets;
        L1.nSets =1;
    }
    if(Memory.L2assoc == 0){
        Memory.L2assoc=L2.nSets;
        L2.nSets=1;
    }

    Block L1Blk[L1.nSets][Memory.L1assoc];
    Block L2Blk[L2.nSets][Memory.L2assoc];

    for(int i = 0; i<L1.nSets; i++){
        for(int j = 0; j<Memory.L1assoc ; j++){
            L1Blk[i][j].tag = -1;
            L1Blk[i][j].lru_index = 0;
        }
    }

    for(int i = 0; i<L2.nSets; i++){
        for(int j = 0; j<Memory.L2assoc ; j++){
            L2Blk[i][j].tag = -1;
            L2Blk[i][j].lru_index = 0;
        }
    }

    L1.access_state = 0 ;
    L2.access_state = 0 ;

    L1.total_reads = 0;
    L1.read_miss = 0;
    L1.total_writes = 0;
    L1.write_miss = 0;
    L1.miss_rate = 0;
    L1.writeback = 0;

    L2.total_reads = 0;
    L2.read_miss = 0;
    L2.total_writes = 0;
    L2.write_miss = 0;
    L2.miss_rate = 0;
    L2.writeback = 0;

    ifstream traces;
    // ofstream tracesout;
    // string output;
    // output = string("trace_small.txt") + ".out";
    traces.open(filename);
    // tracesout.open(output.c_str());

    string line;
    string AS;
    string address;
    int flag = 0;
    int flagR = 0 ;
    unsigned long dirty_tagR =-2;
    unsigned long dirty_tag=-2;
    int Mem_writeback = 0 ;
    int Mem_read =0;
    int l1timestamp;
    int l2timestamp;
    int replace_col = -1;
    int replace_col_2 = -1;

    if(traces.is_open()){
        int timestamp = 0 ;
        while(getline(traces,line)){
            istringstream iss(line);
            if(!(iss >> AS >> address)){break;}
            stringstream saddr(address);

        int addr = hex_to_int(address);

        L1.compute(addr,Memory.L1blocksize,Memory.L1size,Memory.L1assoc);
            int l1tag = L1.blk.tag;
            int l1index = L1.index;
            int l1timestamp = L1.blk.lru_index;
        L2.compute(addr,Memory.L2blocksize,Memory.L2size,Memory.L2assoc);
            int l2tag = L2.blk.tag;
            int l2index = L2.index;
            int l2timestamp = L2.blk.lru_index;
        

        if(AS.compare("r")==0){
            L1.total_reads +=1;
            for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                if(L1Blk[L1.index][L1.counter_col].tag ==L1.blk.tag){
                        L1.same_tag=1;
                        count_1 = L1.counter_col;
                        break;
                    }
            } 
            if(L1.same_tag==1){
                if(L1Blk[L1.index][count_1].data==1){
                  L1.blk.valid_bit =1;
                }}
            if(L1.blk.valid_bit==1 && L1.same_tag==1){
                L1.access_state =ReadHit;
                L1Blk[L1.index][count_1].lru_index = timestamp;
                l1timestamp = L1Blk[L1.index][count_1].lru_index;
            }
            else{
                L1.access_state = ReadMiss;
                L1.read_miss += 1;
                int data_count = 0 ;
                    for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                        if(L1Blk[L1.index][L1.counter_col].data == 1){data_count+=1;}
                    }
                
                if(data_count == Memory.L1assoc){
                    long int min_lru_index = 10000000000 ; 
                    Block replace_blk ;

                    for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                        if(L1Blk[L1.index][L1.counter_col].lru_index<min_lru_index)
                        {min_lru_index = L1Blk[L1.index][L1.counter_col].lru_index;
                        replace_col = L1.counter_col;
                        replace_blk = L1Blk[L1.index][L1.counter_col];}}
                        // cout << "Min lru index is " << min_lru_index << endl;
                    if(replace_blk.dirty_bit == 1){
                            flagR = 1;
                            dirty_tagR = replace_blk.tag;
                            
                        }
                    
                    L1Blk[L1.index][replace_col].tag = L1.blk.tag;
                    L1Blk[L1.index][replace_col].data = 1;
                    L1Blk[L1.index][replace_col].valid_bit=1;
                    L1Blk[L1.index][replace_col].lru_index = timestamp;
                    l1timestamp = L1Blk[L1.index][replace_col].lru_index;
                }
                else{
                    for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                        if(L1Blk[L1.index][L1.counter_col].valid_bit== 0){
                            L1Blk[L1.index][L1.counter_col].tag = L1.blk.tag;
                            L1Blk[L1.index][L1.counter_col].data = 1;
                            L1Blk[L1.index][L1.counter_col].valid_bit =1;
                            L1Blk[L1.index][L1.counter_col].lru_index = timestamp;
                            l1timestamp = L1Blk[L1.index][L1.counter_col].lru_index;
                        break;}}
                }}
        //-----------------------------------L2 Read --------------------------------//
        if(L1.access_state == ReadHit){
            L2.access_state = NotApplicable;
        }
        else if(L1.access_state == ReadMiss){
            L2.total_reads += 1;
            if(flagR == 1){
                    // L1.writeback += 1;
                    for(L2.counter_col = 0; L2.counter_col<Memory.L2assoc; L2.counter_col+=1){
                    if((L2Blk[L2.index][L2.counter_col].tag*(Memory.L2size/(Memory.L2blocksize*Memory.L2assoc))) + L2.index ==(dirty_tagR*(Memory.L1size/(Memory.L1blocksize*Memory.L1assoc)))+L1.index){
                        L2Blk[L2.index][L2.counter_col].dirty_bit = 1;
                        L2.total_writes+=1;
                        L1.writeback+=1;
                        L2Blk[L2.index][L2.counter_col].lru_index = timestamp;
                        break;
                    }}
                }
            for(L2.counter_col = 0; L2.counter_col<Memory.L2assoc; L2.counter_col+=1){
                    if(L2Blk[L2.index][L2.counter_col].tag == L2.blk.tag){
                        L2.same_tag = 1;
                        count_2 = L2.counter_col;
                        break;
                    }
                }
            if(L2.same_tag==1){
                    if(L2Blk[L2.index][count_2].data==1){
                        L2.blk.valid_bit=1;
                    }
                }
            if(L2.blk.valid_bit==1 && L2.same_tag==1){
                    L2.access_state = ReadHit;
                    L2Blk[L2.index][count_2].lru_index = timestamp;
                    l2timestamp = L2Blk[L2.index][count_2].lru_index;
                }
            else{
                L2.access_state = ReadMiss;
                Mem_read += 1;
                L2.read_miss +=1; 
                int data_count = 0 ;
                for(L2.counter_col =0 ;L2.counter_col<Memory.L2assoc;L2.counter_col+=1){
                    if(L2Blk[L2.index][L2.counter_col].data == 1){data_count+=1;}
                }
                if(data_count == Memory.L2assoc){
                        long int min_lru_index = 10000000000 ; 
                        Block replace_blk ;
                    for(L2.counter_col =0 ;L2.counter_col<Memory.L2assoc;L2.counter_col+=1){
                     if(L2Blk[L2.index][L2.counter_col].lru_index<min_lru_index)
                        {min_lru_index = L2Blk[L2.index][L2.counter_col].lru_index;
                        replace_col_2 = L2.counter_col;
                        replace_blk = L2Blk[L2.index][L2.counter_col];
                        // cout << "min_lru_index is " << min_lru_index << endl;
                        }}
                        // cout << "Min lru index is " << min_lru_index << endl;
                     if(replace_blk.dirty_bit == 1){
                            L2.writeback+=1;
                            Mem_writeback += 1;
                        }
                    L2Blk[L2.index][replace_col_2].tag = L2.blk.tag;
                    L2Blk[L2.index][replace_col_2].data = 1;
                    L2Blk[L2.index][replace_col_2].valid_bit =1;
                    L2Blk[L2.index][replace_col_2].lru_index = timestamp; 
                    l2timestamp = L2Blk[L2.index][replace_col_2].lru_index;       
                }
                else{
                    for(L2.counter_col =0 ;L2.counter_col<Memory.L2assoc;L2.counter_col+=1){
                        if(L2Blk[L2.index][L2.counter_col].valid_bit == 0){
                            L2Blk[L2.index][L2.counter_col].tag = L2.blk.tag;
                            L2Blk[L2.index][L2.counter_col].data = 1;
                            L2Blk[L2.index][L2.counter_col].valid_bit = 1;
                            L2Blk[L2.index][L2.counter_col].lru_index = timestamp;
                            l2timestamp = L2Blk[L2.index][L2.counter_col].lru_index;
                            break;
                        }}
                }}
        }
        }
    //------------------------------L1 Write ---------------------------------//
    else{
        L1.total_writes +=1;
        for(L1.counter_col=0; L1.counter_col<(Memory.L1assoc); L1.counter_col+=1){
                if(L1Blk[L1.index][L1.counter_col].tag==L1.blk.tag){
                    L1.same_tag = 1;
                    count_1 = L1.counter_col;
                    break;
                }
            }
            if(L1.same_tag == 1){
                if(L1Blk[L1.index][count_1].data == 1){
                    L1.blk.valid_bit=1;
                }
            }

            if(L1.blk.valid_bit == 1 && L1.same_tag == 1){
                L1.access_state = WriteHit;
                L1.blk.dirty_bit = 1;
                L1Blk[L1.index][count_1].lru_index = timestamp;
                l1timestamp = L1Blk[L1.index][count_1].lru_index;
            }
            else{
                L1.access_state = WriteMiss;
                L1.write_miss +=1;
                int data_count = 0 ;
                for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                    if(L1Blk[L1.index][L1.counter_col].data == 1){data_count+=1;}
                }
                if(data_count == Memory.L1assoc){
                    long int min_lru_index = 10000000000 ; 
                        Block replace_blk ;
                    for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                        if(L1Blk[L1.index][L1.counter_col].lru_index<min_lru_index)
                        {min_lru_index = L1Blk[L1.index][L1.counter_col].lru_index;
                        replace_col = L1.counter_col;
                        replace_blk = L1Blk[L1.index][L1.counter_col];
                        }}
                        // cout << "Min lru index is " << min_lru_index << endl;
                    if(replace_blk.dirty_bit == 1){
                            dirty_tag= replace_blk.tag ;
                            flag = 1;
                            L2.total_writes += 1;
                            L1.writeback += 1;
                        }
                    L1Blk[L1.index][replace_col].tag = L1.blk.tag;
                    L1Blk[L1.index][replace_col].data = 1;
                    L1Blk[L1.index][replace_col].dirty_bit = 1;
                    L1Blk[L1.index][replace_col].valid_bit = 1;
                    L1Blk[L1.index][replace_col].lru_index = timestamp;
                    l1timestamp = L1Blk[L1.index][replace_col].lru_index;
                }
            
            else{
                    for(L1.counter_col =0 ;L1.counter_col<Memory.L1assoc;L1.counter_col+=1){
                        if(L1Blk[L1.index][L1.counter_col].valid_bit== 0){
                            L1Blk[L1.index][L1.counter_col].tag = L1.blk.tag;
                            L1Blk[L1.index][L1.counter_col].data = 1;
                            L1Blk[L1.index][L1.counter_col].dirty_bit = 1;
                            L1Blk[L1.index][L1.counter_col].valid_bit =1;
                            L1Blk[L1.index][L1.counter_col].lru_index = timestamp;
                            l1timestamp = L1Blk[L1.index][L1.counter_col].lru_index;
                            break;
                     }}
                }
            }

            //-------------------------L2 Write -------------------------//
            if(L1.access_state == WriteHit){
                L2.access_state = NotApplicable;
            }
            else if( L1.access_state == WriteMiss){
                L2.total_reads += 1;
                if(flag == 1){
                    // L1.writeback+=1;
                    
                    for(L2.counter_col = 0; L2.counter_col<Memory.L2assoc; L2.counter_col+=1){
                    // if(L2Blk[L2.index][L2.counter_col].tag == dirty_tag){
                    if((L2Blk[L2.index][L2.counter_col].tag*(Memory.L2size/(Memory.L2blocksize*Memory.L2assoc))) + L2.index ==(dirty_tagR*(Memory.L1size/(Memory.L1blocksize*Memory.L1assoc)))+L1.index){
                        L2Blk[L2.index][L2.counter_col].dirty_bit = 1;
                        L2.total_writes+=1;
                        L1.writeback+=1;
                    }}
                }
                for(L2.counter_col = 0; L2.counter_col<Memory.L2assoc; L2.counter_col+=1){
                    if(L2Blk[L2.index][L2.counter_col].tag == L2.blk.tag){
                        L2.same_tag = 1;
                        count_2 = L2.counter_col;
                        break;
                    }
                }
                if(L2.same_tag==1){
                    // cout << "in l2 same tag " << endl;
                    if(L2Blk[L2.index][count_2].data==1){
                        L2.blk.valid_bit=1;
                    }
                }
                if(L2.blk.valid_bit==1 && L2.same_tag==1){
                    L2.access_state = WriteHit;
                    L2Blk[L2.index][count_2].lru_index = timestamp;
                    l2timestamp = L2Blk[L2.index][count_2].lru_index;
                }
                else{
                    L2.access_state = WriteMiss;
                    // L2.write_miss+=1;
                    L2.read_miss+=1;
                    int data_count = 0 ;
                    for(L2.counter_col =0 ;L2.counter_col<Memory.L2assoc;L2.counter_col+=1){
                        if(L2Blk[L2.index][L2.counter_col].data == 1){data_count+=1;}
                    }
                    if(data_count == Memory.L2assoc){
                        long int min_lru_index = 10000000000000000 ; 
                        Block replace_blk ;
                    for(L2.counter_col =0 ;L2.counter_col<Memory.L2assoc;L2.counter_col+=1){
                        if(L2Blk[L2.index][L2.counter_col].lru_index<min_lru_index)
                        {min_lru_index = L2Blk[L2.index][L2.counter_col].lru_index;
                        replace_col_2 = L2.counter_col;
                        replace_blk = L2Blk[L2.index][L2.counter_col];
                        }}
                        // cout << "Min lru index is " << min_lru_index << endl;
                    if(replace_blk.dirty_bit == 1){
                            L2.writeback+=1;
                            Mem_writeback+=1;
                        }
                    for(int i = 0; i < L1.nSets ; i++){
                        for(int j = 0; j < Memory.L1assoc; j++){
                    if((L2Blk[L2.index][replace_col_2].tag*(Memory.L2size/(Memory.L2blocksize*Memory.L2assoc))) + L2.index == ((L1Blk[i][j].tag * (Memory.L1size / (Memory.L1assoc * Memory.L1blocksize)))+ i)){
                        L1Blk[i][j].valid_bit =0 ;
                        L1Blk[i][j].data = 0;
                    }
                    }}
                    L2Blk[L2.index][replace_col_2].tag = L2.blk.tag;
                    L2Blk[L2.index][replace_col_2].data = 1;
                    L2Blk[L2.index][replace_col_2].valid_bit =1;
                    L2Blk[L2.index][replace_col_2].lru_index=timestamp;
                    l2timestamp = L2Blk[L2.index][replace_col_2].lru_index;
                    }
                else{
                    for(L2.counter_col=0;L2.counter_col<Memory.L2assoc;L2.counter_col+=1){
                        if(L2Blk[L2.index][L2.counter_col].valid_bit == 0){
                            L2Blk[L2.index][L2.counter_col].tag = L2.blk.tag;
                            L2Blk[L2.index][L2.counter_col].data = 1;
                            L2Blk[L2.index][L2.counter_col].valid_bit = 1;
                            L2Blk[L2.index][L2.counter_col].lru_index=timestamp;
                            l2timestamp = L2Blk[L2.index][L2.counter_col].lru_index;
                            break;
                        }}
                }
        }
        }}
        // tracesout << L1.access_state << " " <<  L2.access_state << " Tags are "<< l1tag <<" " << l2tag<< " Index are " << l1index << " "<<l2index<< " " << l1timestamp << " " << l2timestamp <<endl;
        L1.same_tag=0;
        L1.readMiss = 0 ;
        L2.same_tag = 0;
        L2.readMiss=0;
        flag = 0;
        flagR = 0;
        dirty_tag = -2;
        dirty_tagR = -2;
        timestamp += 1;
    }
        traces.close();
        cout << "L1 Reads " << L1.total_reads << endl;
        cout << "L1 Read Miss " << L1.read_miss << endl;
        cout << "L1 Writes " << L1.total_writes << endl;
        cout << "L1 Write Miss " << L1.write_miss << endl;
        cout << "L1 Write Back " << L1.writeback << endl;
        cout << "L2 Reads " << L2.total_reads << endl;
        cout << "L2 Read Miss " << L2.read_miss << endl;
        cout << "L2 Writes " << L2.total_writes << endl;
        cout << "L2 Write Miss " << L2.write_miss << endl;
        cout << "L2 Write Back " << L2.writeback << endl;
        cout << "Memory Writeback " << Mem_writeback << endl;
        cout << "Total Access Time is " << (L1.total_reads + L1.total_writes) + 20*(L2.total_reads + L2.total_writes) + 200*(Mem_read + Mem_writeback) << endl;
        // cout.close();
    }
    else {cout <<"Unable to open trace or traceout file";}
    return 0;

}

int main(int argc, char* argv[]){
    // string temp = "cacheconfig.txt";
    // char * tab2 = new char [temp.length()+1];
    // strcpy (tab2,temp.c_str());
    if (argc != 7) {
        std::cout << "Invalid number of arguments.\n";
        return 1;
    }

    // cout << "before execute "<<endl;
    vector<int> inputs;
    string filename;

    for (int i = 1; i < 6; i++) {
        int value;
        istringstream(argv[i]) >> value;
        inputs.push_back(value);}
    
    filename = argv[6];
    execute(inputs,filename);
    return 0;
}






