#include <iostream>
#include "custom_v9.h"
#include "binSearch.h"

template <class T> CustomArrayV9<T>::CustomArrayV9(bool (*c)(T a, T b), 
  int *tU) : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = (T*) new char[allocated * sizeof(T)];
  start = data;
  
  mode = V9_V4MODE;
  targetUpdates = tU;
  
  updates = new Updates<T>(c, V9_MAX_UPDATES);
  buffer = (T*) new char[(V9_MAX_UPDATES + 1) * sizeof(T)];
  
  currUpdates = updatesHandled = 0;
  
  #ifdef V9_DEBUG
  totalUpdates = maxFlushed = 0;
  timesFlushed[0] = timesFlushed[1] = 0;
  #endif
}

template <class T> CustomArrayV9<T>::~CustomArrayV9()
{
  delete[] buffer;
  
  #ifdef V9_DEBUG
  cout<<"Updates per flush: "<<totalUpdates / 
    (timesFlushed[0] + timesFlushed[1])<<endl;
  cout<<"Flushes due to readiness: "<<timesFlushed[0]<<endl;
  cout<<"Flushes due to fullness: "<<timesFlushed[1]<<endl;
  cout<<"Max flushed at once: "<<maxFlushed<<endl;
  #endif
}

template <class T> void CustomArrayV9<T>::ins(T a)
{
  if(mode == V9_V7MODE)
  {
    updates->ins(a, start, size);
    iuh = updates->iuh;
    
    if(updates->firstOverHalf == updates->size)
    {
      w3 = size;
    }
    else
    {
      w3 = updates->indices[updates->firstOverHalf];
    }
    
    if(updates->firstOverHalf == 0)
    {
      w2 = 0;
    }
    else
    {
      w2 = updates->indices[updates->firstOverHalf - 1] + 1;
    }
    
    if(updates->firstOverHalf == 0)
    {
      w1 = 0;
    }
    else
    {
      w1 = updates->indices[0];
    }
    
    updatesAcknowledged++;
    
    currUpdates++;
    if(updates->size == V9_MAX_UPDATES)
      flush(1);
    
    #ifdef V9_DEBUG
    totalUpdates++;
    #endif
    return;
  }
  
  int insertionIndex = 0;
  if(size > 0)
    insertionIndex = binarySearch(start, size, this->comp, a);
  
  bool newlyAllocate = (size == allocated);
  if(insertionIndex < size / 2)
  {
    if(start == data)
      newlyAllocate = true;
  }
  else
  {
    if(start + size == data + allocated)
      newlyAllocate = true;
  }
  
  if(newlyAllocate)
  {
    allocated = allocated * V9_EXPAND_CONST;
    T* newArr = (T*) new char[allocated * sizeof(T)];
    T* newStart = newArr + ((allocated - size) / 2);
    
    for(int i = 0; i < insertionIndex; i++)
    {
      newStart[i] = start[i];
    }
    newStart[insertionIndex] = a;
    for(int i = insertionIndex; i < size; i++)
    {
      newStart[i + 1] = start[i];
    }
    
    size++;
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
  else
  {
    //safe only because size < allocated
    if((insertionIndex < size / 2 && start > data) || 
       (start + size == data + allocated))
    {
      start--;
      for(int i = 0; i < insertionIndex; i++)
      {
        start[i] = start[i+1];
      }
      start[insertionIndex] = a;
    }
    else
    {
      for(int i = size; i > insertionIndex; i--)
      {
        start[i] = start[i-1];
      }
      start[insertionIndex] = a;
    }
    size++;
  }
  
  updatesHandled++;
  updatesAcknowledged++;
  if(*targetUpdates - updatesHandled >= V9_SWITCH_POINT)
  {
    cout<<"V7"<<endl;
    mode = V9_V7MODE;
  }
}

template <class T> void CustomArrayV9<T>::del(int idx)
{
  if(mode == V9_V7MODE)
  {
    updates->del(idx, size);
    iuh = updates->iuh;
    
    if(updates->firstOverHalf == updates->size)
    {
      w3 = size;
    }
    else
    {
      w3 = updates->indices[updates->firstOverHalf];
    }
    
    if(updates->firstOverHalf == 0)
    {
      w2 = 0;
    }
    else
    {
      w2 = updates->indices[updates->firstOverHalf - 1] + 1;
    }
    
    if(updates->firstOverHalf == 0)
    {
      w1 = 0;
    }
    else
    {
      w1 = updates->indices[0];
    }
    
    updatesAcknowledged++;
    
    currUpdates++;
    if(updates->size == V9_MAX_UPDATES)
      flush(1);
    
    #ifdef V9_DEBUG
    totalUpdates++;
    #endif
    return;
  }
  
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
  
  if(size < allocated / (V9_EXPAND_CONST * V9_EXPAND_CONST))
  {
    allocated = allocated / V9_EXPAND_CONST;
    T *newArr = (T*) new char[allocated * sizeof(T)];
    T *newStart = newArr + ((allocated - size) / 2);
    
    for(int i = 0; i < size; i++)
    {
      newStart[i] = start[i];
    }
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
  
  updatesHandled++;
  updatesAcknowledged++;
  if(*targetUpdates - updatesHandled >= V9_SWITCH_POINT)
  {
    cout<<"V7"<<endl;
    mode = V9_V7MODE;
  }
}

//here's hoping this gets optimized out
template <class T> inline void CustomArrayV9<T>::flush()
{
  flush(0);
}

template <class T> void 
  CustomArrayV9<T>::flush(int cause)
{
  if(mode == V9_V4MODE)
  {
    return;
  }
  
  if(updates->size == 0)
  {
    updatesHandled += currUpdates;
    currUpdates = 0;
    return;
  }
  
  #ifdef V9_DEBUG
  maxFlushed = (maxFlushed < updates->size ? updates->size : maxFlushed);
  timesFlushed[cause]++;
  #endif
  
  int insertsUnderHalf = 0;
  int updatesUnderHalf = 0;
  int insertsAfterHalf = 0;
  int updatesAfterHalf = 0;
  
  for(int i = 0; i < updates->size; i++)
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
  
  bool newlyAllocate = 
          (incUnderHalf > 0 && start - incUnderHalf <= data) ||
          (start + size + incAfterHalf >= data + allocated);
  
  T *newArr;
  T *newStart;
  if(newlyAllocate)
  {
    while(newlyAllocate)
    {
      allocated = allocated * V9_EXPAND_CONST;
      newlyAllocate = 
         (incUnderHalf > 0 && ((allocated - size) / 2) < incUnderHalf) ||
         (((allocated - size) / 2) + size + incAfterHalf >= allocated);
    }
    
    newArr = (T*) new char[allocated * sizeof(T)];
    int s = ((allocated - size) / 2) - incUnderHalf;
    newStart = newArr + s;
    
    int writeIdx = 0;
    int readIdx = 0;
    for(int i = 0; i < updates->size; i++)
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
    
    handyLock.lock();
    delete[] data;
    data = newArr;
    start = newStart;
    iuh = 0;
    w0 = 0;
    w1 = size / 2;
    w2 = size / 2;
    w3 = size;
    handyLock.unlock();
  }
  else
  {
    int writeIdx;
    int readIdx;
    int numUpdates;
    int numInserts;
    
    int bufferSize;
    int inBuffer;
    int bufferStart;
    
    //######## UNDER HALF: ########
    if(updatesUnderHalf > 0)
    {
      numUpdates = updatesUnderHalf;
      numInserts = insertsUnderHalf;
      bufferSize = numUpdates + 1;
      inBuffer = 0;
      bufferStart = 0;
      
      writeIdx = 0;
      readIdx = 0;
      newStart = start - incUnderHalf;
      
      int i = 0;
      while(i < bufferSize && readIdx < size)
      {
        buffer[i] = start[readIdx];
        readIdx++;
        i++;
      }
      inBuffer = i;
      
      handyLock.lock();
      w1 = 0;
      handyLock.unlock();
      
      for(int i = 0; i < numUpdates; i++)
      {
        while(writeIdx < updates->indices[i])
        {
          newStart[writeIdx] = buffer[bufferStart];
          writeIdx++;
          w0++;
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx < size)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
        if(updates->types[i] == UPDATE_INSERT)
        {
          newStart[writeIdx] = updates->values[i];
          writeIdx++;
          w0++;
        }
        else
        {
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx < size)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
      }
    }
    
    //######## AFTER HALF: #########
    if(updatesAfterHalf > 0)
    {
      numUpdates = updatesAfterHalf;
      numInserts = insertsAfterHalf;
      bufferSize = numUpdates + 1;
      inBuffer = 0;
      bufferStart = 0;

      writeIdx = updates->indices[updatesUnderHalf];
      readIdx = writeIdx - incUnderHalf;
      newStart = start - incUnderHalf;
      
      int idx = 0;
      while(idx < bufferSize && readIdx < size)
      {
        buffer[idx] = start[readIdx];
        readIdx++;
        idx++;
      }
      inBuffer = idx;
      
      for(int i = updatesUnderHalf; i < updates->size; i++)
      {
        while(writeIdx < updates->indices[i])
        {
          newStart[writeIdx] = buffer[bufferStart];
          writeIdx++;
          w3++; //MOVE THE BARRIER BACK
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx < size)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
        if(updates->types[i] == UPDATE_INSERT)
        {
          newStart[writeIdx] = updates->values[i];
          writeIdx++;
          w3++;
        }
        else
        {
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx >= 0)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
      }
      while(inBuffer > 0)
      {
        newStart[writeIdx] = buffer[bufferStart];
        writeIdx++;
        w3++;
        inBuffer--;
        bufferStart++;
        if(bufferStart == bufferSize)
        {
          bufferStart = 0;
        }
        if(readIdx < size)
        {
          buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
          inBuffer++;
          readIdx++;
        }
      }
    }
    
    size += incUnderHalf + incAfterHalf;
    handyLock.lock();
    start = newStart;
    iuh = 0;
    w0 = 0;
    w1 = size / 2;
    w2 = size / 2;
    w3 = size;
    handyLock.unlock();
  }
  
  updates->empty();
  
  updatesHandled += currUpdates;
  currUpdates = 0;
  
  if(*targetUpdates - updatesHandled < V9_SWITCH_POINT)
  {
    cout<<"V4"<<endl;
    mode = V9_V4MODE;
  }
}

template <class T> T CustomArrayV9<T>::lookup(int idx)
{
  if(mainHasLock)
  {
    if(!(idx < w1)) //either idx < w0 or w2 <= idx < w3
    {
      idx -= iuh;
    }
  }
  T ret = start[idx];
  if(mainHasLock)
  {
    handyLock.unlock();
    mainHasLock = false;
  }
  return ret;
}

template <class T> inline bool CustomArrayV9<T>::ready(int numUpdates,
                                                       int idx)
{
  if(numUpdates != updatesAcknowledged)
  {
    return false;
  }
  if(numUpdates == updatesHandled)
  {
    return true;
  }
  if(mode == V9_V4MODE)
  {
    return false;
  }
  
  if(!mainHasLock)
  {
    handyLock.lock();
    mainHasLock = true;
  }
  
  bool ret = (idx < w1) || 
             (idx < w0) || //even though we use index offsetting
             (idx >= w2 && idx < w3);
  if(!ret)
  {
    handyLock.unlock();
    mainHasLock = false;
  }
  return ret;
}
