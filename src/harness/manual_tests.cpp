#include <iostream>

#include "trees/rbtree.h"
#include "sortedCollection/sortedCollection.h"

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
  
//  int inserts[size] = {5, 4, 8, -3, 2, 11, 7, 25, -113, 1, 3, 0};
//  int sorted[size] = {-113, -3, 0, 1, 2, 3, 4, 5, 7, 8, 11, 25};
  
  RBTree<int> rbt(&intComp);

  cout<<"Sizeof RBNode == "<<sizeof(RBNode<int>)<<endl<<endl;
  
  for (int i = 0; i < size; i++)
  {
    rbt.insert(inserts[i]);
    if (!rbt.verify())
    {
      cout<<"Error: tree verification failed after insertion"<<endl;
      cout<<"  at insertion of "<<inserts[i]<<endl;
      return false;
    }
    rbt.traverse();
  }

  for(int i = 0; i < size; i++)
  {
    RBNode<int> *n = rbt.lookupByIdx(i);
    if (n == nullptr)
    {
      cout<<"Error in lookupByIdx!"<<endl;
      cout<<"  could not find element at index "<<i<<endl;
      return false;
    }
    cout<<n->val<<endl;
    if (n->val != sorted[i])
    {
      cout<<"Error: indexed value not in sorted order"<<endl;
      cout<<"  expected "<<sorted[i]<<", got "<<n->val<<endl;
    }
  }
  if (rbt.lookupByIdx(size) != nullptr)
  {
    cout<<"Error in lookupByIdx! (out of bounds doesn't work)"<<endl;
  }
  for(int i = 0; i < size; i++)
  {
    int idx = (i+((size+1)%2))%2;
    rbt.removeByIdx(idx);
    if (!rbt.verify())
    {
      cout<<"Error: tree verification failed after removal"<<endl;
      cout<<"  at removal of index "<<i<<endl;
      return false;
    }
  }
    
  return true;
}

void runManualTests()
{
  const int size = 15;
  int additions[size] = {5,4,5,2,9,
                         1, -3, 9, 22, 490,
                         11, 12, -7345353, 12, 18};
  if (!testRbtIndexing()) return;

  SortedCollection<int> sc(&intComp);

  for(int i = 0; i < size; i++)
  {
    sc.ins(additions[i]);
  }
  
  sc.del(1);
  sc.del(13);
  
  cout<<"sorted collection:"<<endl;
  
  for(int j = 0; j < size - 2; j++)
  {
    int res = sc.lookup(j);
    cout<<res<<endl;
  }
  cout<<endl;
  
}

