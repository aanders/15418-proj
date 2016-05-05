#!/usr/bin/awk -f
#
# Screenscrape output of repeated sortedCollection runs
# and create CSV output row(s) for timings

BEGIN { matching = 0; }

/Cache/             { matching = 0 }
/SortedCollection/  { matching = 1; printf $4"," }
/RedBlackTree/      { matching = 2; printf $4"," }

/Time/ {
    sub("s","",$8)
    if (matching) { printf $8"," }
}
/insert/ {
    sub("ms","",$5)
    if (matching) { printf $5"," }
}

/lookup/ {
    sub("ms","",$5)
    if (matching) { printf $5"," }
}

/delete/ {
    sub("ms","",$5)
    if (matching == 1) { printf $5"," }
    if (matching == 2) { print $5 }
}

