
#ifndef _RUNNER_H_
#define _RUNNER_H_

#include <fstream>
#include <string>
#include <vector>

#include "sortedCollection/sortedCollection.h"

class Runner
{
  protected:
    std::ifstream& tracefile_;
    
    // The following fields will be populated when running the trace
    unsigned int trial_no_;
    unsigned int line_no_;

  public:
    std::vector<std::string> trialNames;

    Runner(std::ifstream& tracefile) : tracefile_(tracefile) {}
    Runner(std::ifstream& tracefile, std::vector<std::string> trialNames)
      : tracefile_(tracefile), trialNames(trialNames) {}

    ~Runner() { tracefile_.close(); }
    
    virtual void runop(std::string op, std::string data) = 0;

    /*
     * Run a given number of trials on the associated tracefile.
     * The trial number (from 0 to trials-1) will be passed as
     * an argument to the runop function, which allows different
     * behavior to be implemented on each trial.
     */
    void run(unsigned int trials);

    /*
     * Retrieve the name of the associated trial number, if it exists
     */
    std::string getTrialName(unsigned int trial);
};

#endif // _RUNNER_H_
