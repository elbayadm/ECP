
#include "queue.hpp"
#include <iostream>

PriorityQueue::PriorityQueue(int size)
  {
    this->pq = new Node[size+1];
    this->N = 0;
  }

PriorityQueue::~PriorityQueue()
  {
    delete pq;
  }

void PriorityQueue::push(Node node)
  {
    pq[++N] = node;
    swim(N);
  }

bool PriorityQueue::isEmpty()
  { return N == 0; }

Node PriorityQueue::delMin()
  {
    Node min = pq[1];
    exch(1, N--);
    sink(1);
    pq[N+1].id = 0;
    pq[N+1].dist = 0;
    return min;
  }

void PriorityQueue::swim(int k)
  {
    while (k > 1 && less(k/2, k))
    {
      exch(k, k/2);
      k = k/2;
    }
  }

void PriorityQueue::sink(int k)
  {
    while(2*k <= N)
    {
      int j = 2*k;
      if (j < N && less(j, j+1)) j++;
      if (!less(k, j)) break;
      exch(k, j);
      k = j;
    }
  }

void PriorityQueue::exch(int i, int j)
  {
    Node t = pq[i];
    pq[i] = pq[j];
    pq[j] = t;
  }

bool PriorityQueue::Contains(Node node)
  {
    for(int i=0; i < N+1; i++) {
      if (pq[i].id == node.id) {
        return true;
      }
    }
    return false;
  }

bool PriorityQueue::less(int i, int j)
  {
    return (pq[i].dist > pq[j].dist);
  }

void PriorityQueue::update(Node node)
  {
    for(int i=0; i < N+1; i++) {
      if (pq[i].id == node.id) {

        if ( pq[i].dist > node.dist ) {
          pq[i].dist = node.dist;
          exch(i,N);
          swim(N);
        }
        return;
      }
    }
  }
