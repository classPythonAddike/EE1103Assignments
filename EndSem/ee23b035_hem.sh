#!/bin/bash

NUM_POINTS=30;
rm -f ee23b035_hem.txt;

for ((i = 2; i <= NUM_POINTS + 1; i++))
do
    let i_plus_one=1+$i;
    i_val=`./ee23b035_hem $i`;
    i_plus_one_val=`./ee23b035_hem $i_plus_one`;
    ratio=`bc <<< "scale=10; $i_plus_one_val / $i_val"`;

    echo $i $i_val $ratio >> ee23b035_hem.txt;
done;

gnuplot -p -c "ee23b035_hem.gpi";
