#include <iostream>
#include "custom_v6.h"
#include "binSearch.h"

template <class T> CustomArrayV6<T>::CustomArrayV6(bool (*c)(T a, T b))
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = (T*) new char[allocated * sizeof(T)];
  start = data;
  
  numUpdates = 0;
  updates = new Updates<T>(c, V6_MAX_UPDATES);
  
  #ifdef V6_DEBUG
  totalInserts = maxFlushed = 0;
  timesFlushed[0] = timesFlushed[1] = 0;
  #endif
}

#ifdef V6_DEBUG
template <class T> CustomArrayV6<T>::~CustomArrayV6()
{
  cout<<"Inserts per flush: "<<totalInserts / 
    (timesFlushed[0] + timesFlushed[1])<<endl;
  cout<<"Flushes due to readiness: "<<timesFlushed[0]<<endl;
  cout<<"Flushes due to fullness: "<<timesFlushed[1]<<endl;
  cout<<"Max flushed at once: "<<maxFlushed<<endl;
}
#endif

template <class T> void CustomArrayV6<T>::ins(T a)
{
  updates->ins(a, start, size);
  numUpdates++;
  
  if(numUpdates == V6_MAX_UPDATES)
    flush(1);
  
  #ifdef V6_DEBUG
  totalInserts++;
  #endif
}

template <class T> void CustomArrayV6<T>::del(int idx)
{
  updates->del(idx);
  numUpdates++;
  
  if(numUpdates == V6_MAX_UPDATES)
    flush(1);
  
  #ifdef V6_DEBUG
  totalInserts++;
  #endif
}

//here's hoping this gets optimized out
template <class T> void CustomArrayV6<T>::flush()
{
  flush(0);
}

template <class T> void 
  CustomArrayV6<T>::flush(int cause)
{
  if(numUpdates == 0)
    return;
  
  #ifdef V6_DEBUG
  maxFlushed = (maxFlushed < numUpdates ? numUpdates : maxFlushed);
  timesFlushed[cause]++;
  #endif
  
  int insertsUnderHalf = 0;
  int updatesUnderHalf = 0;
  int insertsAfterHalf = 0;
  int updatesAfterHalf = 0;
  
  int heightUnder = 0;
  int heightAfter = 0;
  int allTimeLowUnder = 0;
  int allTimeLowAfter = 0;
  
  for(int i = 0; i < numUpdates; i++)
  {
    if(updates->indices[i] < size / 2)
    {
      if(updates->types[i] == UPDATE_INSERT)
      {
        insertsUnderHalf++;
      }
      updatesUnderHalf++;
    }
    else
    {
      if(updates->types[i] == UPDATE_INSERT)
      {
        insertsAfterHalf++;
      }
      updatesAfterHalf++;
    }
  }
  
  int incUnderHalf = 2*insertsUnderHalf - updatesUnderHalf;
  int incAfterHalf = 2*insertsAfterHalf - updatesAfterHalf;
  
  int startOffset = -incUnderHalf;
  
  bool newlyAllocate = 
          (incUnderHalf > 0 && start + startOffset - incUnderHalf <= data) ||
          (start + startOffset + size + incAfterHalf >= data + allocated);
  
  T *newArr;
  T *newStart;
  if(newlyAllocate)
  {
    while(newlyAllocate)
    {
      allocated = allocated * V6_EXPAND_CONST;
      newlyAllocate = 
         (incUnderHalf > 0 && (allocated / 3) + startOffset < incUnderHalf) ||
         ((allocated / 3) + size + startOffset + incAfterHalf >= allocated);
    }
  }
    
    newArr = (T*) new char[allocated * sizeof(T)];
    newStart = newArr + (allocated / 3) + startOffset;
    
    int writeIdx = 0;
    int readIdx = 0;
    for(int i = 0; i < numUpdates; i++)
    {
      while(writeIdx < updates->indices[i])
      {
        newStart[writeIdx] = start[readIdx];
        readIdx++;
        writeIdx++;
      }
      if(updates->types[i] == UPDATE_INSERT)
      {
        newStart[writeIdx] = updates->values[i];
        writeIdx++;
      }
      else
      {
        readIdx++;
      }
    }
    
    size += incUnderHalf + incAfterHalf;
    while(writeIdx < size)
    {
      newStart[writeIdx] = start[readIdx];
      writeIdx++;
      readIdx++;
    }
    
    delete[] data;
    data = newArr;
    start = newStart;
  
  /*
  else
  {
    int bufferSize = (deletesUnderHalf > deletesAfterHalf ? 
                      deletesUnderHalf : deletesAfterHalf);
    
    start += startOffset;
    
    T buffer[bufferSize];
    int inBuffer = 0;
    int bufferStart = 0;
    
    int writeIdx = 0;
    int readIdx = -startOffset;
    
    for(int i = 0; i < deletesUnderHalf; i++)
    {
      buffer[i] = start[readIdx];
      readIdx++;
    }
    
    for(int i = 0; i < updatesUnderHalf; i++)
    {
      while(writeIdx < updates->indices[i])
      {
        
        start[] = start[j + numUnderHalf - i];
        j++;
      }
      start[j] = inserts->lookup(i);
      j++;
    }
    
    size += incUnderHalf + incAfterHalf;
    j = size - 1;
    for(int i = numInserts - 1; i >= numUnderHalf; i--)
    {
      while(j > insertionIndices[i])
      {
        start[j] = start[j - i + numUnderHalf - 1];
        j--;
      }
      start[j] = inserts->lookup(i);
      j--;
    }
  }
  */
  
  numUpdates = 0;
  updates->empty();
}
/*
template <class T> void CustomArrayV6<T>::del(int idx)
{
  flush(1);
  
  if(idx < size / 2)
  {
    for(int i = idx; i > 0; i--)
    {
      start[i] = start[i-1];
    }
    start++;
  }
  else
  {
    for(int i = idx; i < size - 1; i++)
    {
      start[i] = start[i+1];
    }
  }
  
  size--;
  
  if(size < allocated / (V6_EXPAND_CONST * V6_EXPAND_CONST))
  {
    allocated = allocated / V6_EXPAND_CONST;
    T *newArr = (T*) new char[allocated * sizeof(T)];
    T *newStart = newArr + (allocated / 3);
    
    for(int i = 0; i < size; i++)
    {
      newStart[i] = start[i];
    }
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
}
*/

template <class T> T CustomArrayV6<T>::lookup(int idx)
{
  return start[idx];
}
