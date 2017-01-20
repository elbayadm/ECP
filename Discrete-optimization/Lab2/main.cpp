//
//  main.cpp
//  Lab2
//
//  Created by Maha on 20/04/2015.
//  Copyright (c) 2015 Maha. All rights reserved.
//

#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>


#include "prim.hpp"
#include "kruskal.hpp"

#include <vector>

using namespace std;
const int nv=100;
clock_t start_t,end_t;
double elapsed_p=0, elapsed_k=0;

vector<Edge> readtxt(vector<vertex*>& Heap,string filename){
    int next,j;
    string line;
    vector<Edge> E=*new vector<Edge>;
    
    Heap.resize(nv);
    for (int i=0; i<nv; i++)
      Heap[i] = new vertex(i);

    ifstream infile (filename.c_str());
    int temp,i=0;
    while (getline(infile, line)) {
        if (i==0) {i++; continue;}
        j=0;
        if (line.empty()) continue;
        istringstream iss(line);
        while (iss >> next){
            temp=next;
            if (temp!=0){
                Edge e1=Edge(i-1, j,temp);
                Edge e2=Edge(j, i-1, temp);
                //Preprocessing for Prim:
                e1.neighbour=Heap[j];
                e2.neighbour=Heap[i-1];
                Heap[i-1]->neighbours.push_back(e1);
                Heap[j]->neighbours.push_back(e2);
                
                //Preprocessing for Kruskal
                E.push_back(e1);
            }
            j++;
        }
        i++;
    }
    
    return E;
}


int main() {
    bool do_print=false;
    #ifdef DOPRINT
        do_print = true;
    #endif
    
    for (int fileid=0;fileid<100;fileid++){
        string filename="./testing/testing_graph_"+to_string(fileid)+".txt";
        vector<vertex*> Heap;
        vector<Edge> Edges;
        
        Edges=readtxt(Heap,filename);
        start_t=clock();
        Prim(Heap,fileid, do_print);
        end_t=clock();
        elapsed_p+=(double)(end_t-start_t);
        
        start_t=clock();
        Kruskal(Edges,fileid, do_print);
        end_t=clock();
        elapsed_k+=(double)(end_t-start_t);
            }
    double prim_t=elapsed_p/CLOCKS_PER_SEC/100;
    double kruskal_t=elapsed_k/CLOCKS_PER_SEC/100;

    ofstream outfile ("runtime.txt");
    outfile << "Prim: "<<prim_t << " seconds"<< endl;
    outfile << "Kruskal: "<<kruskal_t << " seconds"<< endl;
    return 0;
}
