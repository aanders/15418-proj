#ifndef __queue_h__
#define __queue_h__

template <class T> class QueueNode
{
  public:
  T val;
  QueueNode<T> *next;
};

template <class T> class Queue
{
  private:
  QueueNode<T> *start;
  QueueNode<T> *end;
  QueueNode<T> *reclaim;
  
  public:
  Queue();
  ~Queue();
  void insert(T t);
  bool remove(T* ret);
};

#include "queue.tpp"
#endif
