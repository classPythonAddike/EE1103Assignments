#!/bin/bash

gcc -o main main.c -lm;

NUMPOINTS=10;
STEP=$((1.0/$NUMPOINTS));
for i in $( seq 1 $NUMPOINTS )
do
  echo $(( $i * $STEP ));
done;
