
//  kruskal.cpp


#include "kruskal.hpp"

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

int FindParent(vector<subset> &subsets, int i)
{
    if (subsets[i].parent != i)
        subsets[i].parent = FindParent(subsets, subsets[i].parent);

    return subsets[i].parent;
}


void Union(vector<subset> &subsets, int x, int y)
{
    int xroot = FindParent(subsets, x);
    int yroot = FindParent(subsets, y);

    // Attach smaller rank tree under root of high rank tree
    // (Union by Rank)
    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;

    // If ranks are same, then make one as root and increment
    // its rank by one
    else
    {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}


bool compareEdgeWeight(Edge e1, Edge e2)
{
    return e1.weight < e2.weight;
}

vector<Edge> Kruskal(vector<Edge> &Edges, int fileid, bool do_print)
{
    ofstream report_k;
    vector<Edge> MST;
    int iter=0;
    vector<vector<int>> adjMat(100, vector<int>(100));
    if(do_print){
        for (int i=0;i<100;i++)
            for (int j; j<100; j++)
                adjMat[i][j]=0;

        string  reportname="kruskal_"+to_string(fileid)+".txt";
        report_k.open (reportname);
            report_k<<iter++<<endl;
            for(int i=0;i<100;i++){
                for(int j=0; j<99;j++)
                    report_k << adjMat[i][j]<<" ";
                report_k<<adjMat[i][99]<< endl;
            }
        }

    int V = 100;
    int e = 0;

    // Sort all the edges in non-decreasing order of their weight
    std::sort(Edges.begin(),Edges.end(), compareEdgeWeight);

    // Allocate memory for creating V subsets
    vector<subset> subsets;
    subsets.resize(V);

    // Create V subsets with single elements
    for (int v = 0; v < V; ++v)
    {   subset sub;
        sub.parent=v;
        sub.rank=0;
        subsets[v]=sub;    }

    // Number of edges in the MST V-1
    int i=0;
    while (e < V -1)
    {
        //Pick the smallest edge:
        Edge next_edge = Edges[i++];

        int x = FindParent(subsets, next_edge.id1);
        int y = FindParent(subsets, next_edge.id2);

        // If including this edge does't cause cycle, include it
        if (x != y)
        {
            e++;
            MST.push_back(next_edge);
            if(do_print){
                if (next_edge.id1>next_edge.id2) adjMat[next_edge.id1][next_edge.id2]=next_edge.weight;
                else adjMat[next_edge.id2][next_edge.id1]=next_edge.weight;
                }

            Union(subsets, x, y);
            if (do_print){
                report_k<<iter++<<endl;
                for(int i=0;i<100;i++){
                    for(int j=0; j<99;j++)
                        report_k << adjMat[i][j]<<" ";
                    report_k<<adjMat[i][99]<< endl;
                    }
                }

        }
        // Else discard the next_edge
    }

    report_k.close();
    return MST;
}
