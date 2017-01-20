
//Dijkstra's algorithm: shorthest Path
//from a start vertex to every other vertex in the Graph.
//Queuing with a naive min-priority queue.

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

#include "dijkstra.hpp"
#include "queue.hpp"





void dijkstra(graph* G, int source, double* dist, bool do_print){
    priority_queue P;
    priority_queue_element tmp;
    int u,v,n;
    double d;
    std::string reportname="dijkstra"+std::to_string(source)+".txt";

    Edge* edge;

    bool visited[G->nb_vertices]={false};

    P.nb_members = 0;
    P.elements   = NULL;

    // Initialize dist and add all vertices to Q:
        for (int i = 0; i < G->nb_vertices; i++) {
        if (i == source)
            dist[i] = 0;
        else
            dist[i] = INFINITY;
        tmp.u = i;
        tmp.dist = dist[i];
        priorityQueueInsert(&P, tmp);
    }

    if (do_print){
        std::ofstream report (reportname, std::ios_base::app);
        print_report(report, dist,G->nb_vertices);
    }


     while (!priorityQueueIsEmpty(&P)) {
        tmp = priorityQueueExtractMin(&P);
        u = tmp.u;
        visited[u]=true;
         // explore all edges going out of u:
         n = G->adjacency_list_array[u].nb_members;
         edge = G->adjacency_list_array[u].head;

         for (int i = 0; i < n; i ++) {
              v = edge->vertex;
              if (!visited[v]) {
                 d = dist[u] + edge->weight;
                 if (d < dist[v]) {
                    dist[v] = d;
                    tmp.u = v;
                    tmp.dist = d;
                    priorityQueueUpdate(&P, tmp);
                }

           }
           edge = edge->next;
       }
    if (do_print){
        std::ofstream report (reportname, std::ios_base::app);
        print_report(report, dist,G->nb_vertices);
        }
    }
    free(P.elements);
}

