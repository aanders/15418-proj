#include <iostream>
#include "sort.h"
#include "custom_v3.h"
#include "binSearch.h"

template <class T> CustomArrayV3<T>::CustomArrayV3(bool (*c)(T a, T b)) 
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = new T[allocated];
  
  numInserts = 0;
  numDeletes = 0;
  
  //create a chain of nodes
  UpdateNode<T> *n;
  UpdateNode<T> *n2 = nullptr;
  for(int i = 0; i < AV3_UPDATE_BUFFER_SIZE; i++)
  {
    n = n2;
    n2 = new UpdateNode<T>;
    n2->next = n;
  }
  
  updates = nullptr;
  notUpdates = n2;
}


template <class T> void CustomArrayV3<T>::addUpdate(int type, T *a, int idx)
{
  UpdateNode<T> *addition = notUpdates;
  notUpdates = notUpdates->next;
  
  addition->type = type;
  if(type == AV3_INS)
  {
    addition->val = *a;
    if(size > 0)
      addition->idx = binarySearch(data, size, this->comp, addition->val);
    else
      addition->idx = 0;
  }
  else if(type == AV3_DEL)
  {
    addition->idx = idx;
  }
  addition->next = nullptr;
  
  if(updates == nullptr)
  {
    updates = addition;
  }
  else
  {
    UpdateNode<T> *list = updates;
    UpdateNode<T> *prevList = nullptr;
    while(list != nullptr && (list->idx < addition->idx || 
      (list->idx == addition->idx && list->type == AV3_DEL) ||
      (list->idx == addition->idx && this->comp(list->val, addition->val))))
    {
      if(addition->type == AV3_INS && list->type == AV3_INS)
      {
        addition->idx++;
      }
      else if(addition->type == AV3_INS && list->idx != addition->idx && 
              list->type == AV3_DEL)
      {
        addition->idx--;
      }
      
      prevList = list;
      list = list->next;
    }
    
    if(prevList == nullptr)
    {
      addition->next = updates;
      updates = addition;
    }
    else if(list == nullptr)
    {
      prevList->next = addition;
    }
    else //could add optimization if it's a delete of the inserted
    {
      addition->next = list;
      prevList->next = addition;
    }
    
    list = addition->next;
    while(list != nullptr)
    {
      if(addition->type == AV3_DEL)
      {
        list->idx--;
      }
      else if(addition->type == AV3_INS)
      {
        list->idx++;
      }
      
      list = list->next;
    }
  }
  
  if(notUpdates == nullptr)
  {
    executeUpdates();
  }
}

template <class T> void CustomArrayV3<T>::ins(T a)
{
  numInserts++;
  addUpdate(AV3_INS, &a, 0);
}

template <class T> void CustomArrayV3<T>::del(int idx)
{
  numDeletes++;
  addUpdate(AV3_DEL, nullptr, idx);
}

/*
{
  for(int i = idx; i < size - 1; i++)
  {
    data[i] = data[i+1];
  }
  
  size--;
  
  if(size < allocated / 4)
  {
    T *newArr = new T[allocated / 2];
    for(int i = 0; i < size; i++)
    {
      newArr[i] = data[i];
    }
    
    delete[] data;
    data = newArr;
  }
}
*/

template <class T> void CustomArrayV3<T>::executeUpdates()
{  
  if(updates == nullptr)
  {
    numInserts = 0;
    numDeletes = 0;
    return;
  }
  
  int newSize = size + numInserts - numDeletes;
  bool newArr = false;
  T *otherData = data;
  
  if(newSize > allocated)
  {
    newArr = true;
    while(newSize > allocated)
    {
      allocated = allocated * 2;
    }
    otherData = new T[allocated];
  }
  
  UpdateNode<T> *u = updates;
  UpdateNode<T> *uPrev = nullptr;
  int currIdx, nextIdx;
  int dataOffset = 0;
  
  T accumulations[numInserts];
  int numAcc = 0;
  int accStart = 0;
  
  while(u != nullptr)
  {
    currIdx = u->idx;
    nextIdx = (u->next != nullptr ? u->next->idx : newSize);
    
    if(u->type == AV3_INS)
    {
      dataOffset--;
    }
    else if(u->type == AV3_DEL)
    {
      dataOffset++;
    }
    
    for(int i = currIdx; i < nextIdx; i++)
    {
      if(dataOffset < 0)
      {
        accumulations[(accStart + numAcc) % numInserts] = data[i + dataOffset];
        numAcc++;
      }
      
      if(numAcc > 0)
      {
        otherData[i] = accumulations[accStart];
        accStart = (accStart + 1) % numInserts;
        numAcc--;
      }
      else
      {
        otherData[i] = data[i + dataOffset];
      }
    }
    
    uPrev = u;
    u = u->next;
  }
  
  //Finishing Touches:
  numInserts = 0;
  numDeletes = 0;
  
  if(newArr)
  {
    delete[] data;
    data = otherData;
  }
  
  uPrev->next = notUpdates;
  notUpdates = updates;
  updates = nullptr;
  
  /*
  //cout<<"Before:\n";
  //printData(data, size);
  
  //if(numInsertions == 0)
  //  return;
  
  //sort(insertions, numInsertions, this->comp);
  //cout<<"Sorted insertions:\n";
  //printData(insertions, numInsertions); 
  
  //int startIdx = 0;
  //int indices[numInsertions];
  
  if(size == 0)
  {
    for(int i = 0; i < numInsertions; i++)
      indices[i] = i;
  }
  else
  {
    for(int i = 0; i < numInsertions; i++)
    {
      startIdx = binarySearch(data, size, 
                              this->comp, insertions[i]);
      indices[i] = startIdx + i;
    }
  }
  
  if(size + numInsertions > allocated)
  {
    //cout<<"Neither here\n";
    while(size + numInsertions > allocated)
    {
      allocated = allocated * 2;
    }
    T *newArr = new T[allocated];
    int offset = 0;
    for(int i = 0; i < size + numInsertions; i++)
    {
      if(offset < numInsertions && i == indices[offset])
      {
        newArr[i] = insertions[offset];
        offset++;
      }
      else
      {
        newArr[i] = data[i - offset];
      }
    }
    
    delete[] data;
    data = newArr;
  }
  else
  {
    //cout<<"Nor there.\n";
    int idx = numInsertions - 1;
    for(int i = size + numInsertions - 1; i >= indices[0]; i--)
    {
      if(i == indices[idx])
      {
        data[i] = insertions[idx];
        idx--;
      }
      else
      {
        data[i] = data[i - (idx + 1)];
      }
    }
  }
  
  size += numInsertions;
  numInsertions = 0;
  //cout<<"After:\n";
  //printData(data, size);
  */
}

template <class T> T CustomArrayV3<T>::lookup(int idx)
{
  executeUpdates();
  return data[idx];
}

template <class T> void CustomArrayV3<T>::printData(T *d, int s)
{
  for(int i = 0; i < s; i++)
  {
    cout<<d[i];
    if(i < s - 1)
      cout<<", ";
  }
  cout<<endl;
}

/*
template <class T> bool CustomArrayV3<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
*/
