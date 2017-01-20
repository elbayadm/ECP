// Graph structure


#include "graph.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>



// Create an empty graph with n vertices
  graph *createGraph(int n) {
      graph *G = new graph;
      G->nb_vertices = n;

      // Create an array of adjacency lists
      G->adjacency_list_array = new adjacency_list[n];

      for(int i = 0; i < n; i++) {
          G->adjacency_list_array[i].head = NULL;
          G->adjacency_list_array[i].nb_members = 0;
      }

      return G;
  }


// Add an edge to a graph.
  void addEdge(graph *G, int src, int dest, double weight) {
      // Create a new adjacency list node
      Edge *newNode = new Edge [1];
      newNode->vertex = dest;
      newNode->weight = weight;

      // Add it in place of the head of the list
      newNode->next = G->adjacency_list_array[src].head;

      G->adjacency_list_array[src].head = newNode;
      G->adjacency_list_array[src].nb_members++;
  }


// Print the txt report
  void print_report( std::ofstream& file, double *dist, int n) {
      for (int i=0; i<n;i++)
      { if(dist[i]==INFINITY)
          file<< "inf ";
          else
          file<< dist[i]<<" ";
      }
      file << std::endl;
  }
