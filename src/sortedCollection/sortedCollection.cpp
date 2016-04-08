#include "queue.cpp"
#include "sortedCollection.h"

template <class T> bool SortedCollection<T>::comp2(Entry<T> a, Entry<T> b)
{
  return comp(a.val, b.val);
}

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b)) 
{
  numUpdates = numVUpdates = numSUpdates = 0;
  comp = c;
  set = std::set<T, bool(*)(T,T)>(comp2);
}

template <class T> void SortedCollection<T>::ins(T t)
{
  numUpdates++;
  Update<T> addition;
  addition.ins = true;
  addition.val = t;
  //setUpdates.ins(addition);
  vectorUpdates.ins(addition);
}


template <class T> void SortedCollection<T>::del(int idx)
{
  numUpdates++;
  Update<T> deletion;
  deletion.ins = false;
  deletion.idx = idx;
  //setUpdates.ins(deletion);
  vectorUpdates.ins(addition);
}


template <class T> T SortedCollection<T>::lookup(int idx)
{
  while(numSVUpdates != numUpdates) {}
  
  return vector[idx];
}

//Returns the first index equal to val
template class<T> int binarySearch(std::vector<T> *vec, bool (*comp)(T a, T b),
                                   T val)
{
  int start = 0;
  int end = vec->size();
  int mid = (end - start) / 2 + start;
  
  while(start != end - 1)
  {
    //equal
    if(!comp(val, (*vec)[mid]) && !comp((*vec)[mid], val))
    {
      end = mid;
    }
    else if(comp(val, (*vec)[mid])) //val < vec[mid]
    {
      end = mid;
    }
    else //val > vec[mid]
    {
      start = mid;
    }
  }
  
  return start;
}

template <class T> void SortedCollection<T>::handleUpdatesVector()
{
  Update<T> u;
  while(true)
  {
    while(!vectorQueue.remove(&u)) {}
    if(!u.ins)
    {
      vector->erase(u.idx);
    }
    else
    {
      
    }
    numVUpdates++;
  }
}
