
//Dijkstra's algorithm: shorthest Path
//from a start vertex to every other vertex in the Graph.
//Queuing with a binary heap priority queue.

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include "queue.hpp"
#include "dijkstra.hpp"
using namespace std;

void dijkstra(graph* G, int source, double* dist, bool do_print){

    int u,v,n;
    double d;
    std::string reportname="dijkstra"+std::to_string(source)+".txt";

    Edge* edge;

    bool visited[G->nb_vertices]={false};
    PriorityQueue P=PriorityQueue(G->nb_vertices);

    // Initialize dist and add all vertices to Q:
        for (int i = 0; i < G->nb_vertices; i++) {
        if (i == source)
            dist[i] = 0;
        else
            dist[i] = INFINITY;
        P.push(Node(i,dist[i]));
    }

    if (do_print){
        std::ofstream report (reportname, std::ios_base::app);
        print_report(report, dist,G->nb_vertices);
    }


     while (!P.isEmpty()) {
        Node tmp =P.delMin();
        u = tmp.id;
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
                    P.update(Node(v,d));
                }

           }
           edge = edge->next;
       }
    if (do_print){
        std::ofstream report (reportname, std::ios_base::app);
        print_report(report, dist,G->nb_vertices);
        }
    }
    }

