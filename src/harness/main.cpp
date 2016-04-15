#include <iostream>
#include <stdlib.h>
#include "trees/rbtree.h"
#include "sortedCollection/sortedCollection.h"

using namespace std;

bool intComp(int a, int b)
{
  return a < b;
}

bool charComp(char a, char b)
{
  return a < b;
}

bool testRbtIndexing()
{
  const int size = 12;
  int inserts[size] = {5, 4, 8, -3, 2, 4, 7, 25, -113, 0, 3, 0};
  int sorted[size] = {-113, -3, 0, 0, 2, 3, 4, 4, 5, 7, 8, 25};
  
  RBTree<int> rbt(&intComp);
  
  for (int i = 0; i < size; i++)
  {
    rbt.insert(inserts[i]);
  }

  for(int i = 0; i < size; i++)
  {
    RBNode<int> *n = rbt.lookupByIdx(i);
    if (n == nullptr)
    {
      cout<<"Error in lookupByIdx!"<<endl;
      return false;
    }
    cout<<n->val<<endl;
    if (n->val != sorted[i])
    {
      cout<<"Error: indexed value not in sorted order"<<endl;
    }
  }
  if (rbt.lookupByIdx(size) != nullptr)
  {
    cout<<"Error in lookupByIdx!"<<endl;
  }
  return true;
}


int main(void)
{
  const int size = 15;
  int additions[size] = {5,4,5,2,9,
                         1, -3, 9, 22, 490,
                         11, 12, -7345353, 12, 18};
  int tests[size] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int refsol[size] = {1,1,0,1,1,0,0,0,1,0,1,1,0,0,0};
  
  if (!testRbtIndexing()) return 0;
  
  SortedCollection<int> sc(&intComp);

  for(int i = 0; i < size; i++)
  {
    sc.ins(additions[i]);
    for(int j = 0; j < i + 1; j++)
    {
      int res = sc.lookup(j);
      cout<<res<<endl;
    }
    cout<<endl;
  }
  for(int i = 0; i < size; i++)
  {
    if (sc.lookupElt(tests[i]) && (refsol[i] == 1))
    {
      cout<<"RBT test "<<i<<" passed! (found element)"<<endl;
    }
    else if (!sc.lookupElt(tests[i]) && (refsol[i] == 0))
    {
      cout<<"RBT test "<<i<<" passed! (element doesn't exist)"<<endl;
    }
    else
    {
      cout<<"RBT test "<<i<<" failed :("<<endl;
    }
  }
  return 0; 
  
}
