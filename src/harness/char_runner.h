
#ifndef _CHAR_RUNNER_H_
#define _CHAR_RUNNER_H_

#include <fstream>
#include <string>

#include "runner.h"

#include "sortedCollection/sortedCollection.h"

class CharRunner : public Runner
{
  protected:
    static bool comp(char a, char b) { return a < b; };
    SortedCollection<char> collection_;

  public:
    CharRunner(std::ifstream& tracefile)
      : Runner(tracefile), collection_(&comp) {}
    
    void runop(std::string op, std::string data, unsigned int trial);
};

#endif // _INT_RUNNER_H_
