#include "vector_v1.h"

//finds FIRST index with an element <= to val
template <class T> int binarySearch(std::vector<T> *vec, bool (*comp)(T a, T b),
                                   T val)
{
  int start = 0;
  int end = vec->size();

  while(start < end - 1)
  {
    int mid = (end - start) / 2 + start;

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

template <class T> Array<T>::Array(bool (*c)(T a, T b))
{
  comp = c;
}

template <class T> void Array<T>::ins(T a)
{
  data.insert(data.begin() + binarySearch(&data, comp, a), a);
}

template <class T> void Array<T>::del(int idx)
{
  data.erase(data.begin() + (idx - 1));
}

template <class T> T Array<T>::lookup(int idx)
{
  return data[idx];
}
