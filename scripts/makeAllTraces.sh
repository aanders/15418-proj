#!/bin/bash
#
# Create a bunch of trace files using the generator with
# different parameter sets.  The format of given parameters
# is slightly different for this script, and more useful for
# generating large numbers of traces with different
# parameter combinations.
#

DELAYS=( "100" "500" "1000" "5000" )
BIASES=( "uniform" "insert" "lookup" "burst" )
LENGTHS=( "16000" "64000" "128000" "512000" "1024000" "2048000" )

#
# Main generator script
#

TRACEDIR=traces
COUNTER=0

function usage() {
    echo "usage: $0 <output-dir>"
    echo "  <output-dir> : subdirectory to be created under the main traces/"
    echo "                 directory, which will hold the generated traces"
    echo "                 The program will exit with an error if <output-dir>"
    echo "                 already exists."
    echo ""
}

if [[ $# < 1 ]] ; then
    usage
    exit 1
fi

if [ -d $TRACEDIR/$1 ] ; then
    echo "Output destination $TRACEDIR/$1 already exists!  Aborting."
    exit 2
else
    mkdir -p $TRACEDIR/$1
fi

TOTAL=$((${#LENGTHS[@]}*${#BIASES[@]}*${#DELAYS[@]}))
for l in "${LENGTHS[@]}"; do
    for b in "${BIASES[@]}"; do
        for d in "${DELAYS[@]}"; do
            paramset="-l $l -b $b -d $d -t int -s $(($d/10))"
            name="$((l/1000))k-int-${d}-tight-$b.trace"
            COUNTER=$(($COUNTER + 1))
            echo ""
            echo -e "\033[1mGenerating trace $COUNTER/$TOTAL\033[0m"
            scripts/tracegen.py $paramset \> $TRACEDIR/$1/$name
        done
    done
done
