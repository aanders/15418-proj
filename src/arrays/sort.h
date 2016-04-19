#ifndef __sort_h__
#define __sort_h__

template <class T> void sort(T *tbs, int size, bool (*comp)(T a, T b))
{
  if(size < 2)
    return;
  
  int idx1 = 0;
  int idx2 = size / 2;
  sort(tbs, idx2, comp);
  sort(tbs + idx2, size - idx2, comp);
  
  int mainIdx = 0;
  //T tmp;
  
  int idx2Orig = idx2;
  T newArr[size];
  while(idx2 < size && idx1 < idx2Orig)
  {
    if(comp(tbs[idx2], tbs[idx1]))
    {
      newArr[mainIdx] = tbs[idx2];
      idx2++;
    }
    else
    {
      newArr[mainIdx] = tbs[idx1];
      idx1++;
    }
    mainIdx++;
  }
  if(idx1 < idx2Orig)
  {
    while(mainIdx < size)
    {
      newArr[mainIdx] = tbs[idx1];
      idx1++;
      mainIdx++;
    }
  }
  else if(idx2 < size)
  {
    while(mainIdx < size)
    {
      newArr[mainIdx] = tbs[idx2];
      idx2++;
      mainIdx++;
    }
  }
  
  
  for(int i = 0; i < size; i++)
  {
    tbs[i] = newArr[i];
  }
  /*
  while(mainIdx < size)
  {
    if(comp(tbs[idx2], tbs[idx1]))
    {
      if(idx1 == mainIdx)
        idx1 = idx2;
      tmp = tbs[mainIdx];
      tbs[mainIdx] = tbs[idx2];
      tbs[idx2] = tmp;
      idx2++;
    }
    else
    {
      if(mainIdx == idx1)
      {
        idx1++;
      }
      else
      {
        tmp = tbs[mainIdx];
        tbs[mainIdx] = tbs[idx1];
        tbs[idx1] = tmp;
      }
    }
    
    if(idx2 == size)
      return;
    
    mainIdx++;
  }
  */
}

#endif
