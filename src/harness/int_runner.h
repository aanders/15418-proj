
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
    SortedCollection<int> *collection_;
    RBTree<int> *tree_;

  public:
    IntRunner(std::ifstream& tracefile);
    ~IntRunner();
   
    void runop(std::string op, std::string data);
    void cleanup();
    void run();
};

#endif // _INT_RUNNER_H_
