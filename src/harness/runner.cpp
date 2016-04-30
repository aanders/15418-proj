#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <unistd.h>

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

void Runner::init() {}
void Runner::cleanup() {}

volatile int spinCounter;

void Runner::run(unsigned int trials)
{
  std::string line;

  std::vector<std::string>::const_iterator it = trialNames.cbegin();
  for (unsigned int t = 0; t < trials; t++)
  {
    trial_no_ = t;
    tracefile_.clear();
    tracefile_.seekg(0, ios::beg);
    line_no_ = 0;
    init();
    double start = CycleTimer::currentSeconds();
    double instr_time = 0.0;
    while (std::getline(tracefile_, line))
    {
      line_no_++;
      std::size_t n = line.find(" ");
      if (n != string::npos)
      {
        std::string op = line.substr(0,n);
        std::string data = line.substr(n+1);
        if (op.compare("pause") == 0)
        {
          try
          {
            int cycles = std::stoi(data);
            for(spinCounter = 0; spinCounter < cycles; spinCounter++) {}
            //usleep(std::stoi(data));
          }
          catch (const std::invalid_argument& e)
          {
            std::cerr << "ERROR: internal error: invalid data for instruction" << std::endl;
            std::cerr << "  at '" << op << " " << data << "'" << std::endl;
            std::cerr << "  at line " << line_no_ << std::endl;
            std::cerr << "  in \"" << getTrialName(trial_no_)
                << "\"" << std::endl;
          }
        }
        else
        {
          double instr_start = CycleTimer::currentSeconds();
          runop(op, data);
          double instr_end = CycleTimer::currentSeconds();
          instr_time += (instr_end - instr_start);
        }
      }
    }
    double end = CycleTimer::currentSeconds();
    cleanup();
    
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
    std::cout << "  --> " << instr_time
        << " seconds spent in data structure operations" << std::endl;
    it++;
  }
}

