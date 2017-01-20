#ifndef Queue_H_
#define Queue_H_

struct Node{
public:
  Node(int id=0, double dist=0) : id(id), dist(dist) {}
  int id;
  double dist;
};

class PriorityQueue
{
public:
  PriorityQueue(int size);
  ~PriorityQueue();
  void push(Node node);
  bool isEmpty();
  Node delMin();
  void swim(int k);
  bool Contains(Node node);
  void update(Node node);
  void show();

private:
  Node* pq; // Priority Queue
  int N;    // queue count

  void sink(int k);
  void exch(int i, int j);
  bool less(int i, int j);
};

#endif