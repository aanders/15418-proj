#!/bin/bash

for i in $( ls $1 );
do
  ./sortedCollection $1/$i | cut -d \  -f 4 >> $2
done
