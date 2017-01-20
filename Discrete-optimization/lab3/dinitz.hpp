//--------------------------------------------------------------------
// Dinitz.hpp
// Dinitz struct
// + Tracking the in-neighbours of the sink (not much of an improvement)
//--------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include <climits>
//#include <iostream>

using namespace std;
ofstream report;
ofstream maxflow;

struct Edge {
    int from, to, cap, flow, index; //index useful to link back to the reversed arc.
    Edge(int from, int to, int cap, int flow, int index):
    from(from), to(to), cap(cap), flow(flow), index(index) {}
};

struct Dinitz {
  int N;
  int src, sink;
  vector<Edge*> inn; //In-neighbours of sink
  int innCount=0;    //the sink in-degre.
  vector<vector<Edge> > G;
  vector<Edge *> parent;
  vector<int> Q;
    //Constructor
    Dinitz(int N) :  N(N), G(N), parent(N), Q(N), inn(N) {
        src=N-2; sink=N-1;
        fill(inn.begin(), inn.end(), (Edge *) NULL);
    }

    //Build Graph adjacency lists
    void AddEdge(int from, int to, int cap) {
        G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
        if(to==sink) {inn[from]=&(G[from].back()); innCount++;}
        //Reversed arc:
        G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
      }


    //Get blocking flow
    int BlockingFlow() {
        //BFS ~ Level graph
        fill(parent.begin(), parent.end(), (Edge *) NULL);
        parent[src] = &G[0][0] - 1;
        int head = 0, tail = 0;
        Q[tail++] = src;
        int touched=0; //Count of visited sink's in-neighbours
        while (head < tail && touched<innCount) {
            int x = Q[head++];
            for (int i = 0; i < (int)G[x].size(); i++) {
                Edge &e = G[x][i];
                if (!parent[e.to] && e.cap - e.flow > 0) {
                    parent[e.to] = &G[x][i];
                    Q[tail++] = e.to;
                    if (inn[e.to]) {touched++; Q.pop_back();}
                }
            }
        }
        if (!touched) return 0;
        //Get the maximum allowable flow on each s-v-t path from the level graph where v is a in-neighbour of t.
        //Return the sum as the blocking flow.
        int totflow = 0;
        for (int i = 0; i < (int)G[sink].size(); i++) {
            Edge *start = &G[G[sink][i].to][G[sink][i].index];
            int maxAflow = INT_MAX;
            for (Edge *e = start; e != parent[src]; e = parent[e->from]) {
                if(maxAflow>e->cap - e->flow){
                    maxAflow = e->cap - e->flow;
                }
            }
            if(!maxAflow) continue;
            //Go back and update the edges in the residual graph
            for (Edge *e = start; e != parent[src]; e = parent[e->from]) {
                e->flow += maxAflow;
                G[e->to][e->index].flow -= maxAflow;
            }
            //Update the innCount
            if(start->cap==start->flow) {innCount--; inn[start->from]=NULL;}
            totflow += maxAflow;
        }
        return totflow;
    }

    //Get max flow
    int GetMaxFlow(bool do_print, int fileid) {
        int flow;
        if(do_print){
            string  reportname="dinit_"+to_string(fileid)+".txt";
            string  maxflowname="dinit_flow_"+to_string(fileid)+".txt";
            report.open (reportname);
            maxflow.open (maxflowname);
            printResidual();
        }
        int totflow = 0;
        while (flow = BlockingFlow()){
            totflow += flow;
            if(do_print) printResidual();
        }
        report.close();
        if (do_print){
            maxflow<<totflow<<endl;
            maxflow.close();
        }
        return totflow;
    }


    void printResidual(){
        int i,j;
        int AdjMat[N][N];
        for(i=0;i<N;i++)
            for(j=0; j<N;j++)
                AdjMat[i][j]=0;

        for(i=0; i<N;i++)
            for (j=0; j<(int)G[i].size(); j++) {
                Edge e=G[i][j];
                if(e.cap-e.flow>0)
                    AdjMat[e.from][e.to]=e.cap-e.flow;
            }

        for(i=0;i<N;i++){
            for(j=0; j<N-1;j++)
                report<<AdjMat[i][j]<<" ";
            report<<AdjMat[i][j]<<endl;
        }
    }

};