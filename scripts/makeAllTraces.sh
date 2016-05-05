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

TRACEDIR=traces/all-tests
COUNTER=0

for l in "${LENGTHS[@]}"; do
    mkdir -p ${TRACEDIR}-$(($l/1000))k
done

TOTAL=$((${#LENGTHS[@]}*${#BIASES[@]}*${#DELAYS[@]}))
for l in "${LENGTHS[@]}"; do
    for b in "${BIASES[@]}"; do
        for d in "${DELAYS[@]}"; do
            paramset="-l $l -b $b -d $d -t int -s $(($d/10))"
            name="$((l/1000))k-int-${d}-tight-$b.trace"
            outfile=${TRACEDIR}-$((l/1000))k/$name
            COUNTER=$(($COUNTER + 1))
            echo ""
            echo -e "\033[1mGenerating trace $COUNTER/$TOTAL\033[0m"
            if [ -e $outfile ] ; then
                echo "  Skipping trace generation: file $outfile exists"
            else
                scripts/tracegen.py $paramset > $outfile
            fi
        done
    done
done
