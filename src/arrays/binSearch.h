#ifndef __binSearch_h__
#define __binSearch_h__

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

#endif
