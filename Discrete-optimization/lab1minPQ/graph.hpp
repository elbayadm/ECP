// Graph structure
#ifndef _GRAPH_H_
#define _GRAPH_H_
#include <fstream>

typedef struct Edge {
    int vertex;  // Other end of the edge
    double weight;  // Weight of the edge
    struct Edge *next;  // Pointer to next item in the adjacency linked list
} Edge;

typedef struct adjacency_list {  // Represents a node
    int nb_members;  // Number of members in the adjacency list
    Edge *head;  // Head of the adjacency linked list
} adjacency_list;


typedef struct graph {
    int nb_vertices;  // Number of vertices
    adjacency_list *adjacency_list_array;  // An array of adjacency lists
} graph;

graph *createGraph(int n);
void addEdge(graph *graph, int src, int dest, double weight);

void print_report( std::ofstream& file, double *dist, int n);

#endif