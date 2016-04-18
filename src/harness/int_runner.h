
#ifndef _INT_RUNNER_H_
#define _INT_RUNNER_H_

#include <fstream>
#include <string>

#include "runner.h"

#include "sortedCollection/sortedCollection.h"
#include "trees/rbtree.h"

class IntRunner : public Runner
{
  protected:
    static bool comp(int a, int b) { return a < b; };
    SortedCollection<int> collection_;
    RBTree<int> tree_;

  public:
    IntRunner(std::ifstream& tracefile)
      : Runner(tracefile, {"SortedCollection trial", "RedBlackTree trial"}),
        collection_(&comp), tree_(&comp) {}
    
    void runop(std::string op, std::string data);
    void run();
};

#endif // _INT_RUNNER_H_
