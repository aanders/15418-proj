#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "CycleTimer.h"

#include "sortedCollection/sortedCollection.h"
#include "runner.h"

std::string Runner::getTrialName(unsigned int trial)
{
  if (trial < trialNames.size())
  {
    return trialNames[trial];
  }
  else
  {
    return std::string("Trial ") + std::to_string(trial);
  }
}

void Runner::run(unsigned int trials)
{
  std::string line;
  std::string op;

  std::vector<std::string>::const_iterator it = trialNames.cbegin();
  for (unsigned int t = 0; t < trials; t++)
  {
    tracefile_.clear();
    tracefile_.seekg(0, ios::beg);
    double start = CycleTimer::currentSeconds();
    while (std::getline(tracefile_, line))
    {
      std::size_t n = line.find(" ");
      if (n != string::npos)
      {
        runop(line.substr(0,n), line.substr(n+1), t);
      }
    }
    double end = CycleTimer::currentSeconds();
    
    if (std::distance(it, trialNames.cend()) > 0)
    {
      std::cout << *it << " took " << end-start
          << " seconds to complete" << std::endl;
    }
    else
    {
      std::cout << "Trial " << t << " took " << end-start
          << " seconds to complete" << std::endl;
    }
    it++;
  }
}

