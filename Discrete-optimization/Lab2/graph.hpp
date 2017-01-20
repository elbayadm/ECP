
//  graph.hpp


#ifndef __graph__
#define __graph__

#include <vector>
#include <climits>

struct Edge;

struct vertex {
    unsigned int vertexid;    // Vertex ID
    struct vertex* parent;    // Closest explored parent
    int distance;            // Minimum distance from an explored vertex
    std::vector<Edge> neighbours;  // Neighbouring vertices
    bool searched;            // Flag if already explored
    int rank;

    vertex(unsigned int id) {
        vertexid = id;
        distance = INT_MAX;  // Mark distance of each vertex as "infinite"
        searched = false;
        parent=this;
        rank=0;
    }
    ~vertex();
};

struct Edge {
    int weight;              // Weight of the edge
    vertex* neighbour;       // Neighbouring vertex
    int id1;
    int id2;  //Id of the two vertices.

    Edge()
    {
        id1 = -1;
        id2 = -1;
        weight = -1;
    }

    Edge(int n1, int n2, int w)
    {
        id1 = n1;
        id2 = n2;
        weight = w;
    }
    bool isEmpty()
    {
        return id1 == -1 && id2 == -1 && weight == -1;
    }
};

#endif
