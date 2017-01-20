#ifndef _QUEUE_H_
#define _QUEUE_H_


typedef struct priority_queue_element {
  int u;
  double dist;
} priority_queue_element;

typedef struct priority_queue {
  int nb_members;
  priority_queue_element* elements;
} priority_queue;

priority_queue_element priorityQueueExtractMin(priority_queue* P) ;
void priorityQueueRemove(priority_queue* P, int x) ;
void priorityQueueInsert(priority_queue* P, priority_queue_element e);
void priorityQueueUpdate(priority_queue* P, priority_queue_element e);
int priorityQueueIsEmpty(priority_queue* P);
void priorityQueueFree(priority_queue* P);

#endif
