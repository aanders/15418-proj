/*
 * Like runner.cpp, but loads the entire file into memory and
 * performs all parsing before running the trace.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#include <unistd.h>

#include "CycleTimer.h"

#include "sortedCollection/sortedCollection.h"
#include "trees/rbtree.h"
#include "memrunner.h"

template <class T>
data_t<T> mkdata(T data)
{
  data_t<T> d;
  d.data = data;
  return d;
}

template <class T>
data_t<T> mkintdata(int data)
{
  data_t<T> d;
  d.int_data = data;
  return d;
}

template <class T>
data_t<T> mkalldata(int i, T dd)
{
  data_t<T> d;
  d.int_data = i;
  d.data = dd;
  return d;
}

template <class T>
MemRunner<T>::MemRunner(std::ifstream& tracefile, bool (*c)(T a, T b))
      : comp(c), tracefile_(tracefile),
        trialNames({"Cache loading", "SortedCollection timing", "RedBlackTree reference"}),
        operations(OPERATION_MAPPING)
{
  collection_ = new SortedCollection<T>(c);
  tree_ = new RBTree<T>(c);
}

template <class T>
MemRunner<T>::MemRunner(std::ifstream& tracefile, bool (*c)(T a, T b),
    std::vector<std::string> trialNames) : comp(c), tracefile_(tracefile),
    trialNames(trialNames), operations(OPERATION_MAPPING)
{
  collection_ = new SortedCollection<T>(c);
  tree_ = new RBTree<T>(c);
}

template <class T>
MemRunner<T>::~MemRunner()
{
  if (collection_) delete collection_;
  if (tree_) delete tree_;
}

template <class T>
std::string MemRunner<T>::getTrialName(unsigned int trial)
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

template <class T> void MemRunner<T>::init() {}

template <class T> void MemRunner<T>::cleanup()
{

  if (trial_no_ == 0)
  {
    // Remove the tree we used for prepping the cache
    // Remove the tree we used for prepping the cache
    delete tree_;
    tree_ = new RBTree<T>(comp);
  }
  if (trial_no_ == 1)
  {
    // Remove the sortedCollection so it doesn't get in the
    // way of the reference timing
    delete collection_;
    collection_ = nullptr;
  }
}

template <class T>
void MemRunner<T>::run_insert(unsigned int trial_no, T elt)
{
  if (trial_no == 1)
  {
    collection_->ins(elt);
  }
  else
  {
    tree_->insert(elt);
  }
#ifdef DEBUG
  std::cout << "Inserted " << elt << std::endl;
#endif
}

template <class T>
T MemRunner<T>::run_lookup(unsigned int trial_no, int idx)
{
  T elt;
  if (trial_no == 1)
  {
    elt = collection_->lookup(idx);
  }
  else
  {
    RBNode<int> *node = tree_->lookupByIdx(idx);
    elt = (node != nullptr) ? node->val : 0;
  }
#ifdef DEBUG
  std::cout << "Lookup at index " << idx << std::endl;
  std::cout << "  -> expected " << ref << ", got " << elt << std::endl;
#endif
  return elt;
}

template <class T>
void MemRunner<T>::run_delete(unsigned int trial_no, int idx)
{
  if (trial_no == 1)
  {
    collection_->del(idx);
  }
  else
  {
    tree_->removeByIdx(idx);
  }
#ifdef DEBUG
  std::cout << "Deletion at index " << idx << std::endl;
#endif
}

template <class T>
bool MemRunner<T>::loadTrace(T (*parser)(std::string s))
{
  std::string line;
  bool errors = false;

  line_no_ = 0;
  tracefile_.clear();
  tracefile_.seekg(0, ios::beg);

  while (std::getline(tracefile_, line))
  {
    line_no_++;

    if (line_no_ == 1)
    {
      if (line.find("DATATYPE:") != 0)
      {
        std::cerr << "ERROR: internal error: invalid datatype header found" << std::endl;
        std::cerr << "  at '" << line << "'" << std::endl;
        std::cerr << "  at line " << line_no_ << std::endl;
        errors = true;
      }
      else
      {
        // header OK, continue to next line
        continue;
      }
    }

    // line_no >= 2
    std::size_t n = line.find(" ");
    if (n != string::npos)
    {
      std::string op = line.substr(0,n);
      std::string datastr = line.substr(n+1);

      if (op.compare("p")  == 0 || op.compare("d") == 0)
      {
        try
        {
          int data = std::stoi(datastr);
          trace.emplace_back(std::make_pair(operations[op], mkintdata<T>(data)));
        }
        catch (const std::invalid_argument& e)
        {
          std::cerr << "ERROR: internal error: invalid data for instruction" << std::endl;
          std::cerr << "  at '" << op << " " << datastr << "'" << std::endl;
          std::cerr << "  at line " << line_no_ << std::endl;
          errors = true;
        }
      }
      else if (op.compare("l") == 0)
      {
        std::size_t i = datastr.find(" ");
        if (i != string::npos)
        {
          try
          {
            int idx = std::stoi(datastr.substr(0,i));
            T ref = parser(datastr.substr(i+1));
            trace.emplace_back(std::make_pair(operations[op], mkalldata<T>(idx, ref)));
          }
          catch (const std::invalid_argument& e)
          {
            std::cerr << "ERROR: internal error: invalid data for instruction" << std::endl;
            std::cerr << "  at '" << op << " " << datastr << "'" << std::endl;
            std::cerr << "  at line " << line_no_ << std::endl;
            errors = true;
          }
          catch (const invalid_data& f)
          {
            std::cerr << "ERROR: internal error: invalid reference value for instruction" << std::endl;
            std::cerr << "  at '" << op << " " << datastr << "'" << std::endl;
            std::cerr << "  at line " << line_no_ << std::endl;
            errors = true;
          }
        }
      }
      else if (op.compare("i") == 0)
      {
        try
        {
          T data = parser(datastr);
          trace.emplace_back(std::make_pair(operations[op], mkdata<T>(data)));
        }
        catch (const invalid_data& e)
        {
          std::cerr << "ERROR: internal error: invalid data for instruction" << std::endl;
          std::cerr << "  at '" << op << " " << datastr << "'" << std::endl;
          std::cerr << "  at line " << line_no_ << std::endl;
          errors = true;
        }
      }
      else
      {
        std::cerr << "ERROR: internal error: unrecognized instruction" << std::endl;
        std::cerr << "  at '" << op << " " << datastr << "'" << std::endl;
        std::cerr << "  at line " << line_no_ << std::endl;
        errors = true;
      }
    }
  } // end while
  tracefile_.close();
  return !errors;
}

static volatile int spinCounter;
template <class T>
void MemRunner<T>::run(unsigned int trials)
{
  for (trial_no_ = 0; trial_no_ < trials; trial_no_++)
  {
    double instr_time  = 0.0;
    double insert_time = 0.0;
    double lookup_time = 0.0;
    double delete_time = 0.0;
    long   inserts     = 0;
    long   lookups     = 0;
    long   deletes     = 0;
    
    line_no_ = 0;
    init();
    double start = CycleTimer::currentSeconds();
    for (auto& opdata : trace)
    {
      line_no_++;
      oper_t op = opdata.first;
      data_t<T> data = opdata.second;

      int cycles, idx;
      T elt, ref;
      double instr_start = 0.0, instr_end = 0.0;
      switch (op)
      {
        case PAUSE:
          cycles = data.int_data;
          for(spinCounter = 0; spinCounter < cycles; spinCounter++) {}
          break;

        case INSERT:
          elt = data.data;
          
          instr_start = CycleTimer::currentSeconds();
          run_insert(trial_no_, elt);
          instr_end = CycleTimer::currentSeconds();

          instr_time += (instr_end - instr_start);
          insert_time = (insert_time * inserts) + (instr_end - instr_start);
          insert_time /= ++inserts;

          break;

        case LOOKUP:
          idx = data.int_data;
          ref = data.data;

          instr_start = CycleTimer::currentSeconds();
          elt = run_lookup(trial_no_, idx);
          instr_end = CycleTimer::currentSeconds();

          if (comp(ref, elt) || comp(elt, ref))
          {
            std::cerr << "ERROR: lookup failed: expected " << ref
                << ", got " << elt << std::endl;
            std::cerr << "  at line " << line_no_ << std::endl;
            std::cerr << "  in \"" << getTrialName(trial_no_) << "\"" << std::endl;
          }

          instr_time += (instr_end - instr_start);
          lookup_time = (lookup_time * lookups) + (instr_end - instr_start);
          lookup_time /= ++lookups;

          break;

        case DELETE:
          idx = data.int_data;

          instr_start = CycleTimer::currentSeconds();
          run_delete(trial_no_, idx);
          instr_end = CycleTimer::currentSeconds();

          instr_time += (instr_end - instr_start);
          delete_time = (delete_time * deletes) + (instr_end - instr_start);
          delete_time /= ++deletes;
          break;
      }
    }
    double end = CycleTimer::currentSeconds();
    cleanup();
    
    std::cout << getTrialName(trial_no_) << " took " << end-start
        << " seconds to complete" << std::endl;
    std::cout << "  === Time spent in data structure operations: "
        << instr_time << "s" << std::endl;
    std::cout << "  === Average insert time: "
        << insert_time*1000 << "ms" << std::endl;
    std::cout << "  === Average lookup time: "
        << lookup_time*1000 << "ms" << std::endl;
    std::cout << "  === Average delete time: "
        << delete_time*1000 << "ms" << std::endl;
  }
}
