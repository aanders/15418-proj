#include "queue.h"

#ifndef __queue_cpp__
#define __queue_cpp__

//This code almost verbatum from lecture slides.

template <class T> Queue<T>::Queue()
{
  start = end = reclaim = new QueueNode<T>;
}

template <class T> void Queue<T>::insert(T t)
{
  QueueNode<T> *newNode = new QueueNode<T>;
  newNode->val = t;
  newNode->next = 0;
  
  end->next = newNode;
  end = end->next;
  
  while(reclaim != start)
  {
    QueueNode<T> *tmp = reclaim;
    reclaim = reclaim->next;
    delete tmp;
  }
}

template <class T> bool Queue<T>::remove(T* ret)
{
  if(start != end)
  {
    *ret = start->next->value;
    start = start->next;
    return true;
  }
  return false;
}

#endif
