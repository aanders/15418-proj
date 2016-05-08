#!/bin/bash
#
# Merge all csv files in the result directory

RESULTDIR=results

function usage() {
    echo "usage: $0 <output-dir>"
    echo "  <output-dir> : output directory for merged csv files"
}

if [[ $# < 1 ]]; then
    usage
    exit 1
fi

OUTPUTDIR=$1
mkdir -p $OUTPUTDIR

PREFIXES=$(ls $RESULTDIR | cut -d- -f1-3 | sort -u)
for p in $PREFIXES; do
    scripts/merge_csv.sh $RESULTDIR/$p* > $OUTPUTDIR/$p.csv
done
