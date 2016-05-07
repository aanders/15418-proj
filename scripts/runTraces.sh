#!/bin/bash
#
# Run a collection of traces identified by a
# subdirectory in the main traces folder

TRACEDIR=traces

function usage() {
    echo "usage: $0 [-m] <tracedir> [<outfile>]"
    echo "  -m         : Use in-memory trace runner"
    echo "  <tracedir> : The subdirectory under traces/ contaning the traces to run"
    echo "  <outfile>  : The file to which the trace timings will be appended"
    echo ""
}

if [[ $# < 1 ]] ; then
    usage
    exit 1
fi

if [[ $# > 2 && $1 == "-m" ]]; then
    MEM_FLAG="-m"
    CLI_DIR=$2
    CLI_FILE=$3
elif [[ $# == 2 && $1 == "-m" ]]; then
    MEM_FLAG="-m"
    CLI_DIR=$2
    CLI_FILE=""
elif [[ $# == 2 && $1 != "-m" ]]; then
    MEM_FLAG=""
    CLI_DIR=$1
    CLI_FILE=$2
elif [[ $# = 1 && $1 != "-m" ]]; then
    MEM_FLAG=""
    CLI_DIR=$1
    CLI_FILE=""
else
    usage
    exit 1
fi

if [ ! -d $TRACEDIR/$CLI_DIR ] ; then
    echo "error: $TRACEDIR/$CLI_DIR is not a directory"
fi

for tracefile in $( ls $TRACEDIR/$CLI_DIR/*.trace | sort -t- -k5n -k7r) ; do
  if [[ -z $CLI_FILE ]] ; then
    echo "Running trace: $tracefile"
    ./sortedCollection $MEM_FLAG $tracefile
  else
    ./sortedCollection $tracefile | cut -d \  -f 4 >> $CLI_FILE
  fi
done
