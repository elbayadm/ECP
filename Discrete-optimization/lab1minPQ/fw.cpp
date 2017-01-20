 
//Floyd Warshall algorithm


#include "fw.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>


void floyd_warshall(graph* G, double** dist , bool do_print){

    Edge* edge;
    int n,v;


    //Initialize the distance matrix:
    for (int i = 0; i < G->nb_vertices; i++)
    {
        for (int j = 0; j < G->nb_vertices; j++)
         {
         	if(i==j) dist[i][j]=0;
         	else dist[i][j]=INFINITY;
         }
    }

    for (int i = 0; i < G->nb_vertices; i++)
    {
        edge = G->adjacency_list_array[i].head;
        n = G->adjacency_list_array[i].nb_members;
        for (int j = 0; j < n; j++) {
        	v = edge->vertex;
          dist[i][v]=edge->weight;
          edge = edge->next;
         }
    }


    //Run Floyd Warshall:
    for (int i = 0; i < G->nb_vertices; i++) {
          for (int j = 0; j < G->nb_vertices; j++) {
            if (dist[j][i]==INFINITY) continue;
            for (int k = 0; k < G->nb_vertices; k++){
              if (dist[j][k]>dist[j][i]+dist[i][k])
                dist[j][k]=dist[j][i]+dist[i][k];
            }
          }
        }


    if(do_print){
    	std::ofstream report ("fw.txt");
        for (int i = 0; i < G->nb_vertices; i++)
        {
        for (int j = 0; j < G->nb_vertices; j++){
        	if(dist[i][j]==INFINITY)
        		report<<"inf ";
        	else
        		report<< dist[i][j]<< " ";
        }
        report<<std::endl;
    	}
        report.close();
	   }
   }


