// Bellman Ford algorithm

#include "bf.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <iostream>
#include <fstream>


bool change=false;
void bellman_ford(graph* G, int source, double* dist, bool do_print){

    Edge* edge;
    int n,v;
    double d;
    std::string reportname="bf"+std::to_string(source)+".txt";

    // Initialize dist:
    for (int i = 0; i < G->nb_vertices; i++) {
        if (i == source)
            dist[i] = 0;
        else
            dist[i] = INFINITY;
    }

    if (do_print)
    {
        std::ofstream report (reportname,std::ios_base::app);
        print_report(report, dist,G->nb_vertices);
    }

    for (int i=0;i<G->nb_vertices;i++){
        change=false;
        for(int j=0;j<G->nb_vertices;j++){
            edge = G->adjacency_list_array[j].head;
            n = G->adjacency_list_array[j].nb_members;
            for (int k = 0; k < n; k++) {
                 d = dist[j] + edge->weight;
                 v = edge->vertex;
                 if (dist[v]>d){
                    dist[v]=d;
                    change=true;
                 }
                 edge = edge->next;
            }
        }
        if (!change) break;
         if (do_print)
             {
                std::ofstream report (reportname,std::ios_base::app);
                print_report(report, dist,G->nb_vertices);
                }

    }
}
