#!/usr/bin/awk -f
#
#
# Average lines of multiple CSV files, separated by tabs; i.e.
#
# Input:
#
# CSV_FILE_1_LINE_1     CSV_FILE_2_LINE_1       ...
# CSV_FILE_1_LINE_2     CSV_FILE_2_LINE_2       ...
# ...
#
# Output:
#
# CSV_AVERAGE_LINE_1
# CSV_AVERAGE_LINE_2
# ...
# 
# where the average is the pointwise average of each field among the lines
# from the various files.

BEGIN { FS = "\t"; }

{
    for (i = 1; i <= NF; i++) {
        values[i]["init"] = 0 # forces array, init value never referenced
        num_records = split($i, values[i], ",")
    }
    for (j = 1; j <= num_records; j++ ) {
        avg[j] = 0
        for (i = 1; i <= NF; i++) {
            avg[j] = avg[j] + values[i][j]
        }
        avg[j] = avg[j] / NF
    }
    for (j = 1; j < num_records; j++) {
        printf avg[j]","
    }
    print avg[num_records]
}
