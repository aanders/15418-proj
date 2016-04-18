
#ifndef _RUNNER_H_
#define _RUNNER_H_

#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"

class Runner
{
  protected:
    std::ifstream& tracefile_;

  public:

    Runner(std::ifstream& tracefile) : tracefile_(tracefile) {}
    ~Runner() { tracefile_.close(); }
    
    virtual void runop(std::string op, std::string data) = 0;

    void run();
};

#endif // _RUNNER_H_
