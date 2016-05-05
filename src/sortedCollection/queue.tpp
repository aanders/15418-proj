#include "queue.h"

#ifndef __queue_tpp__
#define __queue_tpp__

//This code almost verbatum from lecture slides.

template <class T> Queue<T>::Queue()
{
  start = end = reclaim = new QueueNode<T>;
}

template <class T> Queue<T>::~Queue()
{
  while(reclaim != start)
  {
    QueueNode<T> *tmp = reclaim;
    reclaim = reclaim->next;
    delete tmp;
  }
}

template <class T> void Queue<T>::insert(T t)
{
  QueueNode<T> *newNode;
  if(reclaim != start)
  {
    newNode = reclaim;
    reclaim = reclaim->next;
  }
  else
  {
    newNode = new QueueNode<T>;
  }
  newNode->val = t;
  newNode->next = 0;
  
  end->next = newNode;
  end = end->next;
  
  /*
  while(reclaim != start)
  {
    QueueNode<T> *tmp = reclaim;
    reclaim = reclaim->next;
    delete tmp;
  }
  */
}

template <class T> bool Queue<T>::remove(T* ret)
{
  if(start != end)
  {
    *ret = start->next->val;
    start = start->next;
    return true;
  }
  return false;
}

#endif
