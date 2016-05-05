#!/bin/bash
#
# Take multiple CSV files and average each data point across the files.
# (See scripts/merge_csv.awk for details).

function usage() {
    echo "usage: $0 <csv-files>"
    echo "  where <csv-files> is a list of CSV files to merge"
}

if [[ $# -lt 1 ]]; then
    usage
    exit 1
fi

paste $@ | scripts/merge_csv.awk

