
//  kruskal.h


#ifndef __kruskal__
#define __kruskal__

#include <stdio.h>
#include <vector>
#include "graph.hpp"
using namespace std;

// A structure to represent a subset for union-find
struct subset
{
    int parent;
    int rank;
};


int FindParent(vector<subset> &subsets, int i);
void Union(vector<subset> &subsets, int x, int y);

vector<Edge> Kruskal(vector<Edge> &Edges, int fileid, bool do_print);

#endif /* defined(__MST__kruskal__) */
