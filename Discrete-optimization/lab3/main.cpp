// Discrete Optimization
// Lab3 - MinCut/MaxFlow with Dinitz's blocking flow algorithm
// Update: Commented version
//--------------------------------------------------------------------

#include "dinitz.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>


using namespace std;
ofstream runtime;
clock_t start_t,end_t;
double elapsed=0;

Dinitz readtxt(string filename){
    int next,j;
    string line;
    Dinitz D=Dinitz(402); //As |V| is not given in the file header (lazy approach)

    ifstream infile (filename.c_str());
    int i=0;
    while (getline(infile, line)) {
        j=0;
        istringstream iss(line);
        while (iss >> next){
            if(next>0) D.AddEdge(i, j,  next);
            j++;
            }
        i++;
    }

    return D;
}

int main() {
    bool do_print=false;
#ifdef DOPRINT
    do_print = true;
#endif
    runtime.open("runtime.txt");
    for (int fileid=0;fileid<100;fileid++){
        string filename="./testing/graph_"+to_string(fileid)+".txt";
        Dinitz D=readtxt(filename);
        start_t=clock();
        D.GetMaxFlow(do_print,fileid);
        end_t=clock();
        elapsed+=(double)(end_t-start_t);
    }
    runtime<<"Dinit: "<<elapsed/CLOCKS_PER_SEC/100<<" seconds"<<endl;

    return EXIT_SUCCESS;
}

