#!/bin/bash
#
# Run a collection of traces identified by a
# subdirectory in the main traces folder

TRACEDIR=traces

function usage() {
    echo "usage: $0 <tracedir> [<outfile>]"
    echo "  <tracedir> : The subdirectory under traces/ contaning the traces to run"
    echo "  <outfile>  : The file to which the trace timings will be appended"
    echo ""
}

if [[ $# < 1 ]] ; then
    usage
    exit 1
fi

if [ ! -d $TRACEDIR/$1 ] ; then
    echo "error: $TRACEDIR/$1 is not a directory"
fi

for tracefile in $( ls $TRACEDIR/$1/*.trace | sort -t- -k5n -k7r) ; do
  if [[ $# < 2 ]] ; then
    echo "Running trace: $tracefile"
    ./sortedCollection $tracefile
  else
    ./sortedCollection $tracefile | cut -d \  -f 4 >> $2
  fi
done
