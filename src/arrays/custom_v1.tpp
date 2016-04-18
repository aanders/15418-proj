#include <iostream>
#include "custom_v1.h"
#include "binSearch.h"

template <class T> CustomArray<T>::CustomArray(bool (*c)(T a, T b)) 
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = new T[allocated];
}

template <class T> void CustomArray<T>::ins(T a)
{
  int insertionIndex = binarySearch(data, size, this->comp, a);
  
  if(size == allocated)
  {
    T* newArr = new T[allocated * 2];
    allocated = allocated * 2;
    for(int i = 0; i < insertionIndex; i++)
    {
      newArr[i] = data[i];
    }
    newArr[insertionIndex] = a;
    for(int i = insertionIndex; i < size; i++)
    {
      newArr[i + 1] = data[i];
    }
    
    size++;
    
    delete[] data;
    data = newArr;
  }
  else
  {
    //safe only because size < allocated
    for(int i = size; i > insertionIndex; i--)
    {
      data[i] = data[i-1];
    }
    data[insertionIndex] = a;
    size++;
  }
}

template <class T> void CustomArray<T>::del(int idx)
{
  for(int i = idx; i < size - 1; i++)
  {
    data[idx] = data[i+1];
  }
  
  size--;
  
  if(size < allocated / 4)
  {
    T *newArr = new T[allocated / 2];
    allocated = allocated / 2;
    for(int i = 0; i < size; i++)
    {
      newArr[i] = data[i];
    }
    
    delete[] data;
    data = newArr;
  }
}

template <class T> T CustomArray<T>::lookup(int idx)
{
  return data[idx];
}

/*
template <class T> bool CustomArray<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
*/
