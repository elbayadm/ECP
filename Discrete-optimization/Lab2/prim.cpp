//  prim.cpp

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "prim.hpp"

ofstream report;

// Vertices comparator
struct Comp
{   //s1 rank < s2 rank
    bool operator()(vertex* s1, vertex* s2)
    {
        return s1->distance > s2->distance;
    }
};


vector<Edge> Prim(vector<vertex*>& Heap,int fileid, bool do_print){
    int iter=0;
    vector<vector<int>> adjMat(100, vector<int>(100));
    vector<Edge> MST;
    if(do_print){
        for (int i=0;i<100;i++)
            for (int j; j<100; j++)
                adjMat[i][j]=0;
        string  reportname="prim_"+to_string(fileid)+".txt";
        report.open (reportname);
        }

    vector<vertex*> Explored;
    Explored.resize(Heap.size());


    // Set source vertex distance
    int source =0;
    Heap[source]->distance = 0;

    // Make a Heap
    make_heap(Heap.begin(), Heap.end(), Comp());

    while (Heap.size() != 0){
        pop_heap (Heap.begin(),Heap.end(), Comp());
        vertex* next_vertex = Heap.back();
        Heap.pop_back();

        next_vertex->searched = true;
        MST.push_back(Edge(next_vertex->vertexid,next_vertex->parent->vertexid,next_vertex->distance));

        if (do_print) // Build the Tree to print
            {if (next_vertex->parent->vertexid!=next_vertex->vertexid)
            {if (next_vertex->parent->vertexid > next_vertex->vertexid)
                    adjMat[next_vertex->parent->vertexid][next_vertex->vertexid]=next_vertex->distance;
                else
                    adjMat[next_vertex->vertexid][next_vertex->parent->vertexid]=next_vertex->distance;
            }
            }

        // Set the minimum distance of each neighbouring vertex
        for(vector<Edge>::iterator it = next_vertex->neighbours.begin(); it < next_vertex->neighbours.end(); it++) {
            if(it->neighbour->searched == true)
            continue;

        // Calculate minimum distance of neighbouring vertex
            if (it->weight < it->neighbour->distance)
            {   it->neighbour->distance =it->weight;
                it->neighbour->parent=next_vertex;
            }
        }
        if (do_print){
            report<<iter++<<endl;
            for(int i=0;i<100;i++){
                for(int j=0; j<99;j++)
                    report << adjMat[i][j]<<" ";
                report<<adjMat[i][99]<< endl;
                }
            }

        // Update the heap order
        make_heap(Heap.begin(),Heap.end(), Comp());
    }
    report.close();
    return MST;
   }

