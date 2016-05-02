#include <iostream>
#include <vector>
#include "array.h"
#include "vector_v1.h"

template <class T> int binarySearch(std::vector<T> *vec, bool (*comp)(T a, T b),
                                   T val)
{
  int start = 0;
  int end = vec->size();
  int mid = (end - start) / 2 + start;
  
  while(start < end - 1)
  {
    mid = (end - start) / 2 + start;
    //equal
    if(!comp(val, (*vec)[mid]) && !comp((*vec)[mid], val))
    {
      return mid;
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
  
  if(comp(val, (*vec)[start]))
  {
    return start;
  }
  
  return start + 1;
}

template <class T> VectorArray<T>::VectorArray(bool (*c)(T a, T b)) 
  : Array<T>(c) {updatesHandled = 0;}

template <class T> void VectorArray<T>::ins(T a)
{
  if(data.size() == 0)
  {
    data.push_back(a);
  }
  else
  {
    int insertionIndex = binarySearch(&data, this->comp, a);
    data.insert(data.begin() + insertionIndex, a);
  }
  updatesHandled++;
}

template <class T> void VectorArray<T>::del(int idx)
{
  data.erase(data.begin() + idx);
  updatesHandled++;
}

template <class T> T VectorArray<T>::lookup(int idx)
{
  return data[idx];
}

template <class T> void VectorArray<T>::flush() {}

template <class T> inline bool VectorArray<T>::ready(int numUpdates, 
                                                      int idx)
{
  return numUpdates == updatesHandled;
}
