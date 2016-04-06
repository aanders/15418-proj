#!/usr/bin/env python2
#
# Generate test traces for evaluating the data structure

import random

TRACE_LENGTH = 100
DATA_RANGE = [0,10]

INSTRUCTIONS = [
    "insert",
    "lookup",
    "delete",
]

def instruction():
    instr = INSTRUCTIONS[random.randint(0,len(INSTRUCTIONS)-1)]
    data = random.randint(*DATA_RANGE)
    return instr, data

if __name__ == '__main__':
    for i in xrange(0,TRACE_LENGTH):
        line = instruction()
        print line[0], line[1]

