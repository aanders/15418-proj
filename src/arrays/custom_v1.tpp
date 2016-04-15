#include <iostream>
#include "custom_v1.h"

template <class T> int binarySearch(T *arr, int size, bool (*comp)(T a, T b),
                                   T val)
{
  int start = 0;
  int end = size;
  int mid = (end - start) / 2 + start;
  
  while(start < end - 1)
  {
    mid = (end - start) / 2 + start;
    //equal
    if(!comp(val, arr[mid]) && !comp(arr[mid], val))
    {
      return mid;
    }
    else if(comp(val, arr[mid])) //val < vec[mid]
    {
      end = mid;
    }
    else //val > vec[mid]
    {
      start = mid;
    }
  }
  
  if(comp(val, arr[start]))
  {
    return start;
  }
  
  return start + 1;
}

template <class T> CustomArray<T>::CustomArray(bool (*c)(T a, T b)) 
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = new T[allocated];
}

template <class T> void CustomArray<T>::ins(T a)
{
  if(size == allocated)
    //do something.
  int insertionIndex = binarySearch(data, size, this->comp, a);
  insertionIndex++;
}

template <class T> void CustomArray<T>::del(int idx)
{
  //implement
  idx++;
}

template <class T> T CustomArray<T>::lookup(int idx)
{
  return data[idx];
}

template <class T> bool CustomArray<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
