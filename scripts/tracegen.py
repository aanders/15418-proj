#!/usr/bin/env python2
#
# Generate test traces for evaluating the data structure

import argparse
import bisect
import random
import sys

TRACE_LENGTH = 100
INT_DATA_RANGE = [-1000,1000]
CHAR_DATA_RANGE = [33, 126] # printable, non-space ASCII range

DATATYPES = ['int', 'char']
DATATYPE = DATATYPES[0] # int by default

INSTRUCTIONS = [
    "insert",
    "lookup",
    "delete",
]

UNIFORM_PDF = [1.0/len(INSTRUCTIONS) for i in xrange(0,len(INSTRUCTIONS))]
INSERT_BIAS_PDF = [0.7, 0.2, 0.1]
LOOKUP_BIAS_PDF = [0.2, 0.6, 0.2]
BIASTYPES = ['uniform', 'insert', 'lookup']
BIAS = BIASTYPES[0] # uniform by default

MEAN_DELAY = 500 # microseconds
STDDEV_DELAY = 200

values = []

# Return a random element of list l based on the
# probability density represented by the list pdf,
# where the elements of pdf sum to 1 and pdf[i]
# represents the probability of choosing l[i]
def rand_elt(l, pdf):
    r = random.uniform(0,1)
    i = 0
    cumulative_p = 0
    if len(l) != len(pdf):
        return None
    for p in pdf:
        cumulative_p += p
        if cumulative_p > r:
            break
        else:
            i += 1
    return l[i]

# Generate a random trace instruction
def instruction():
    # Pick instruction according to pdf
    if BIAS == 'uniform':
        instr = rand_elt(INSTRUCTIONS, UNIFORM_PDF)
    elif BIAS == 'insert':
        instr = rand_elt(INSTRUCTIONS, INSERT_BIAS_PDF)
    elif BIAS == 'lookup':
        instr = rand_elt(INSTRUCTIONS, LOOKUP_BIAS_PDF)

    # Generate appropriate data for the selected instruction
    if instr == 'insert':
        if DATATYPE == 'char':
            data = chr(random.randint(*CHAR_DATA_RANGE))
        else:
            data = random.randint(*INT_DATA_RANGE)
        bisect.insort_left(data, values)
        return instr, data
    elif instr == 'delete':
        if len(values) == 0:
            # can't do delete with no data
            return None, None
        else:
            data = random.randint(0,len(values)-1)
            del values[data]
            return instr, data
    elif instr == 'lookup':
        # TODO: possibly simulate out-of-bounds lookups?
        if len(values) == 0:
            # can't do lookup with no data
            return None, None
        else:
            data = random.randint(0,len(values)-1)
            ref = values[data]
            return instr, data, ref
    else:
        return None, None

# Generate a random pause interval
def pause():
    delay_us = random.gauss(MEAN_DELAY, STDDEV_DELAY)
    return 'pause', abs(int(delay_us))

# Print a variable number of arguments
def printline(*args):
    output = ""
    for a in args:
        output += str(a) + " "
    if output != "":
        output = output[:-1]
    print output

def load_args():
    global TRACE_LENGTH, DATATYPE, BIAS, MEAN_DELAY, STDDEV_DELAY
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--trace-length", type=int,
            metavar="NUM",
            help="Number of instructions to generate (default: 100)")
    parser.add_argument("-t", "--datatype", choices=DATATYPES,
            help="Type of data values to use (default: int)")
    parser.add_argument("-b", "--bias", choices=BIASTYPES,
            help="Bias for the trace (default: uniform)")
    parser.add_argument("-d", "--avg-delay", type=int,
            metavar="MICROSECONDS",
            help="Average delay between instructions (default: 500us)")
    parser.add_argument("-s", "--stddev-delay", type=int,
            metavar="MICROSECONDS",
            help="Std-dev of delay between instructions (default: 300us)")
    args = parser.parse_args()

    if args.trace_length:
        TRACE_LENGTH = args.trace_length
    if args.datatype:
        DATATYPE = args.datatype
    if args.bias:
        BIAS = args.bias
    if args.avg_delay:
        MEAN_DELAY = args.avg_delay
    if args.stddev_delay:
        STDDEV_DELAY = args.stddev_delay
#
# MAIN
#
if __name__ == '__main__':
    load_args()

    # Only print program info if we have stdout redirected
    if not sys.stdout.isatty():
        sys.stderr.write("Beginning trace generation with the following parameters:\n")
        sys.stderr.write("  trace-length = " + str(TRACE_LENGTH) + "\n")
        sys.stderr.write("  data type    = " + str(DATATYPE) + "\n")
        sys.stderr.write("  bias         = " + str(BIAS)     + "\n")
        sys.stderr.write("  avg-delay    = " + str(MEAN_DELAY) + "\n")
        sys.stderr.write("  stddev-delay = " + str(STDDEV_DELAY) + "\n")

    for i in xrange(0,TRACE_LENGTH):
        if not sys.stdout.isatty():
            sys.stderr.write("\rProgress: {}%".format(int(float(i)/float(TRACE_LENGTH)*100)))
        line = (None, None)
        while line[0] == None:
            line = instruction()
        printline(*line)
        line = pause()
        printline(*line)

    if not sys.stdout.isatty():
        sys.stderr.write("\nTrace generation complete.\n")

