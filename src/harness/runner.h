
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
    
    virtual void runop(std::string op, std::string data,
        unsigned int trial) = 0;

    /*
     * Run a given number of trials on the associated tracefile.
     * The trial number (from 0 to trials-1) will be passed as
     * an argument to the runop function, which allows different
     * behavior to be implemented on each trial.
     */
    void run(unsigned int trials);
};

#endif // _RUNNER_H_
