#include <iostream>
#include "custom_v1.h"
#include "binSearch.h"

template <class T> CustomArray<T>::CustomArray(bool (*c)(T a, T b)) 
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = new T[allocated];
  updatesHandled = 0;
}

template <class T> void CustomArray<T>::ins(T a)
{
  int insertionIndex = 0;
  if(size > 0)
    insertionIndex = binarySearch(data, size, this->comp, a);
  
  if(size == allocated)
  {
    T* newArr = (T*) new char[allocated * V1_EXPAND_CONST * sizeof(T)];
    allocated = allocated * V1_EXPAND_CONST;
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
  
  updatesHandled++;
}

template <class T> void CustomArray<T>::del(int idx)
{
  for(int i = idx; i < size - 1; i++)
  {
    data[i] = data[i+1];
  }
  
  size--;
  
  if(size < allocated / (V1_EXPAND_CONST * V1_EXPAND_CONST))
  {
    T *newArr = new T[allocated / V1_EXPAND_CONST];
    allocated = allocated / V1_EXPAND_CONST;
    for(int i = 0; i < size; i++)
    {
      newArr[i] = data[i];
    }
    
    delete[] data;
    data = newArr;
  }
  
  updatesHandled++;
}

template <class T> T CustomArray<T>::lookup(int idx)
{
  return data[idx];
}

template <class T> void CustomArray<T>::flush() {}

template <class T> inline bool CustomArray<T>::ready(int numUpdates, 
                                                    int idx)
{
  return numUpdates == updatesHandled;
}
