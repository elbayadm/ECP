
#include <ctime>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>

#include "graph.hpp"
#include "dijkstra.hpp"
#include "bf.hpp"
#include "fw.hpp"

using namespace std;
string filename= "metroEdgeDist.txt";
graph* parsefile(string filename);


int main(){
    clock_t start_t, end_t;
    double dijk_t,bf_t,fw_t;
    bool do_print = false;
    #ifdef DOPRINT
        do_print = true;
    #endif

    graph *graph = parsefile(filename);

    // Run Dijsktra
    start_t = clock();
    for (int v=0;v<graph->nb_vertices;v++)
    {
        int start = v;
        double *dist = new double[graph->nb_vertices];

        dijkstra(graph, start, dist, do_print);

    }
    end_t=clock();
    dijk_t=(double)(end_t - start_t)/CLOCKS_PER_SEC/304;


    // Run Bellmanford
    start_t = clock();
    for (int v=0;v<graph->nb_vertices;v++)
    {
        int start = v;
        double *dist = new double[graph->nb_vertices];

        bellman_ford(graph, start, dist, do_print);

    }
    end_t=clock();
    bf_t=(double)(end_t - start_t)/CLOCKS_PER_SEC/304;

    // Run Floyd Warshall
    start_t = clock();
    double** dist = new double*[graph->nb_vertices * graph->nb_vertices];
    for(int i = 0; i < graph->nb_vertices; i++)
        dist[i] = new double[graph->nb_vertices];
    floyd_warshall(graph, dist, do_print);
    end_t=clock();
    fw_t=(double)(end_t - start_t)/CLOCKS_PER_SEC;

    ofstream runtime ("runtime.txt");

    runtime << "Dijksktra: " << dijk_t <<" seconds" << endl;
    runtime << "Bellman-Ford: " << bf_t <<" seconds" << endl;
    runtime << "Floyd-Warshall: " << fw_t <<" seconds" << endl;

    runtime.close();

    return 0;
    }


graph* parsefile(string filename){

    graph *graph = NULL;
    int i=0;
    int vertices;
    int edges=0;
    int newV=1;
    int rdeg=0;
    int neighbor=1000;
    double weight;

    ifstream file(filename);
    string str,str1;

        while (getline(file,str)) {
            if (i == 0) {
                vertices=atoi(str.c_str());

                graph = createGraph(vertices);
                i++;
            }
            else{
                if(newV==1){
                    rdeg=atoi(str.c_str());
                    rdeg=2*rdeg;
                    if (rdeg==0) {newV=1; i++;}
                    else newV=0;}


                else{
                    if (rdeg !=0 && rdeg%2==0){
                        neighbor=atoi(str.c_str());
                        rdeg--;
                    }
                    else{
                        weight=atof(str.c_str());
                        addEdge(graph, i-1, neighbor, weight);
                        edges++;
                        rdeg--;
                        if(rdeg==0){
                            newV=1;
                            i++;}
                    }
                }
            }
    }

    file.close();
    cout<<"Total edges: "<<edges<<endl;
    return graph;
    }


