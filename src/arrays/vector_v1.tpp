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
  : Array<T>(c) {}

template <class T> void VectorArray<T>::ins(T a)
{
  if(data.size() == 0)
  {
    data.push_back(a);
  }
  else
  {
    //std::cout<<"A"<<std::endl;
    int insertionIndex = binarySearch(&data, this->comp, a);
    //std::cout<<"B"<<std::endl;
    data.insert(data.begin() + insertionIndex, a);
    //std::cout<<"C"<<std::endl;
  }
  //sortedChecker();
}

template <class T> void VectorArray<T>::del(int idx)
{
  data.erase(data.begin() + idx);
  //sortedChecker();
}

template <class T> T VectorArray<T>::lookup(int idx)
{
  return data[idx];
}

template <class T> void VectorArray<T>::flush() {}

/*
template <class T> void VectorArray<T>::sortedChecker()
{
  bool ok = true;
  int size = data.size();
  
  for(int i = 0; i < size - 1; i++)
  {
    if(this->comp(data[i+1], data[i]))
    {
      ok = false;
    }
    cout<<data[i]<<", ";
  }
  if(size > 0)
    cout<<data[size - 1]<<endl;
  else
    cout<<endl;
  
  if(!ok)
  {
    cout<<"Not ok bro.\n";
  }
}
*/
/*
template <class T> bool VectorArray<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
*/
