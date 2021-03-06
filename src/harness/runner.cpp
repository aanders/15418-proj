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

static volatile int spinCounter;

void Runner::run(unsigned int trials)
{
  std::string line;

  std::vector<std::string>::const_iterator it = trialNames.cbegin();
  for (unsigned int t = 0; t < trials; t++)
  {
    double instr_time  = 0.0;
    double insert_time = 0.0;
    double lookup_time = 0.0;
    double delete_time = 0.0;
    long   inserts     = 0;
    long   lookups     = 0;
    long   deletes     = 0;

    trial_no_ = t;
    line_no_ = 0;
    tracefile_.clear();
    tracefile_.seekg(0, ios::beg);
    init();

    double start = CycleTimer::currentSeconds();
    while (std::getline(tracefile_, line))
    {
      line_no_++;
      std::size_t n = line.find(" ");
      if (n != string::npos)
      {
        std::string op = line.substr(0,n);
        std::string data = line.substr(n+1);
        if (op.compare("p") == 0)
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
          if (op.compare("i") == 0)
          {
            insert_time = (insert_time * inserts) + (instr_end - instr_start);
            insert_time /= ++inserts;
          }
          else if (op.compare("l") == 0)
          {
            lookup_time = (lookup_time * lookups) + (instr_end - instr_start);
            lookup_time /= ++lookups;
          }
          else if (op.compare("d") == 0)
          {
            delete_time = (delete_time * deletes) + (instr_end - instr_start);
            delete_time /= ++deletes;
          }
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
    std::cout << "  === Time spent in data structure operations: "
        << instr_time << "s" << std::endl;
    std::cout << "  === Average insert time: "
        << insert_time*1000 << "ms" << std::endl;
    std::cout << "  === Average lookup time: "
        << lookup_time*1000 << "ms" << std::endl;
    std::cout << "  === Average delete time: "
        << delete_time*1000 << "ms" << std::endl;
    it++;
  }
}

