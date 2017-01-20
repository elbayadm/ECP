#include <cstdio>
#include <cstdlib>
#include "queue.hpp"

priority_queue_element priorityQueueExtractMin(priority_queue* P) {
  float d_min = P->elements[0].dist;
  int i_min = -1;
  priority_queue_element e, e_min;
  for (int i = 0; i < P->nb_members; i++) {
    e = P->elements[i];
    if (e.dist <= d_min) {
      e_min = e;
      i_min = i;
      d_min = e.dist;
    }
  }
  if (i_min == -1){
    fprintf(stderr, "P is empty.\n");
    exit(EXIT_FAILURE);
  }

  priorityQueueRemove(P, i_min);

  return e_min;
}
void priorityQueueRemove(priority_queue* P, int x) {
  P->nb_members --;
  for (int i = x; i < P->nb_members; i++) {
    P->elements[i] = P->elements[i+1];
  }
  if (P->nb_members == 0)
    P = NULL;
  else {
    P->elements = (priority_queue_element*)
      realloc(P->elements,
                   sizeof(priority_queue_element)*P->nb_members);
  }
}

void priorityQueueInsert(priority_queue* P, priority_queue_element e) {

  P->nb_members++;
  P->elements = (priority_queue_element*)
    realloc(P->elements, sizeof(priority_queue_element)*P->nb_members);

  P->elements[P->nb_members-1] = e;
}

void priorityQueueUpdate(priority_queue* P, priority_queue_element e) {
  int u = e.u;
  float d = e.dist;
  for (int i = 0; i < P->nb_members; i++) {
    if (P->elements[i].u == u) { /* update */
      P->elements[i].dist = d;
      return;
    }
  }
  fprintf(stderr, "Node %d not in P.\n", u);
}

int priorityQueueIsEmpty(priority_queue* P) {
  return P->nb_members == 0;
}

void priorityQueueFree(priority_queue* P) {
  free(P->elements);
  free(P);
}
