/*
 * Like runner.h, but loads the entire file into memory and
 * performs all parsing before running the trace.
 */

#ifndef _MEMRUNNER_H_
#define _MEMRUNNER_H_

#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

#include "sortedCollection/sortedCollection.h"

// An operation will be represented as an unsigned integer for fast comparison
enum oper_t { INSERT, LOOKUP, DELETE, PAUSE };

// Initialization list for an unordered map of strings to oper_t values
#define OPERATION_MAPPING {{"p",PAUSE},{"l",LOOKUP},{"i",INSERT},{"d",DELETE}}

// The data associated with an operation can be an integer index or a T
// (or both, in the case of a lookup)
template <class T>
struct data_t
{
  int int_data;
  T data;
};

template <class T>
class MemRunner
{
  protected:
    bool (*comp)(T a, T b);
    SortedCollection<T> *collection_;
    RBTree<T> *tree_;

    std::ifstream& tracefile_;

    std::vector<std::pair<oper_t, data_t<T>>> trace;

    // The following fields will be populated when running the trace
    unsigned int trial_no_;
    unsigned int line_no_;

  public:
    std::vector<std::string> trialNames;

    // Map operation names to their corresponding integers
    std::unordered_map<std::string, oper_t> operations ;

    class invalid_data : public std::exception {};

    MemRunner(std::ifstream& tracefile, bool (*c)(T a, T b));
    MemRunner(std::ifstream& tracefile, bool (*c)(T a, T b), std::vector<std::string> trialNames);
    ~MemRunner();

    /*
     * Read the tracefile into memory, and pre-parse the instructions
     * and data.  This method may only be called once.  Returns true
     * if no errors were encountered; otherwise returns false.
     *
     * The provided parser function should parse an appropriate datatype
     * from the argument string.  If the string is unparseable, the function
     * may raise an instance of MemRunner::invalid_data
     */
    bool loadTrace(T (*parser)(std::string s));

    /*
     * These functions will be called at the beginning and end of
     * each "trial" execution respectively.  The trial number
     * can be obtained from the trial_no_ variable.
     *
     * The time taken by these functions is not included in the
     * reported trace running time.
     */
    virtual void init();
    virtual void cleanup();

    virtual void run_insert(unsigned int trial_no, T data);
    virtual T run_lookup(unsigned int trial_no, int idx);
    virtual void run_delete(unsigned int trial_no, int idx);
    
    /*
     * Run the in-memory trace runner trials times
     * (trial_no = 0 to trials-1).
     */
    void run(unsigned int trials);

    /*
     * Retrieve the name of the associated trial number, if it exists
     */
    std::string getTrialName(unsigned int trial);

};

#include "memrunner.tpp"
#endif // _MEMRUNNER_H_


