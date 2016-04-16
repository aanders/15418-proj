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
  T tmp;
  
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
    
    if(idx1 == idx2 || idx2 == size)
      return;
    
    mainIdx++;
  }
}

#endif
