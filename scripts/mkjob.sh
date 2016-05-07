#!/bin/bash
#
# Master script for generating timings
#
# Generate timing CSV data file for a set of trace files


# PARAMS
TRACEDIR=traces
RESULTDIR=results
SCSRC=src/sortedCollection/sortedCollection.tpp
MAKELOG=make.log
TRACESETS=$(find $TRACEDIR -mindepth 1 -maxdepth 1 -type d -printf "%f\n" | sort -t- -k3n)


echo ""
echo "  MASTER TRACE SET RUNNER"
echo ""

echo "Please select a trace set to run:"
select traceset in $TRACESETS; do
    if [[ "$traceset" != "" && -e $TRACEDIR/$traceset ]] ; then
        echo "Using trace set '$traceset'"
        break
    else
        echo "Invalid selection."
    fi
done

ver=$(cat $SCSRC | fgrep 'define ARRAY_VERSION' | grep -oP 'ARRAY_VERSION \K\d')

echo ""
echo "The current array version is V$ver"
echo "Enter a new array version to use, or hit Enter to use the current version"
while true; do
    echo -n "Please enter an array version [4/8/9]: "
    read newver
    case $newver in
        "")
            echo "Array version unchanged"
            break
            ;;
        4|8|9)
            echo "Using CustomArrayV$newver (recompiling)"
            break
            ;;
        *) echo "Only versions 4, 8, and 9 can be tested." ;;
    esac
done

if [ -n "$newver" ]; then
    cat $SCSRC | sed 's/\(define\ ARRAY_VERSION\ \)[0-9]/\1'$newver'/' > $SCSRC.tmp
    mv -f $SCSRC.tmp $SCSRC
    make > $MAKELOG 2>&1
    if [ "$?" != 0 ]; then
        echo "Compilation failed!  Check $MAKELOG for errors"
        exit 1
    fi
    rm -f $MAKELOG
    ver=$newver
fi

while true; do
    echo -n "Run in memory? [y/N]: "
    read mem
    case $mem in
        y|Y|yes|Yes|YES)
            runner="mem"
            memflag="-m"
            break
            ;;
        n|N|no|No|NO|"")
            runner="disk"
            memflag=""
            break
            ;;
        *) echo "Please answer y or n." ;;
    esac
done

mkdir -p $RESULTDIR

tracelen=$(echo $traceset | cut -d- -f3)
resultFilePrefix=v$ver-$runner-$tracelen
mostRecentTrial=$(find $RUNNER -name $resultFilePrefix-*.csv -printf "%f\n" | cut -d- -f4 | cut -d. -f1)
if [ -z "$mostRecentTrial" ]; then
    trial=1
else
    trial=$((1+$mostRecentTrial))
fi

#
# Finally, actually run the job
#
resultfile=$RESULTDIR/$resultFilePrefix-$(printf "%02d" $trial).csv
echo "Running trace set '$traceset', output to '$resultfile'"
scripts/runTraces.sh $memflag $traceset | scripts/parse_csv.awk > $resultfile
