#!/bin/bash
for i in BRAVO pfq; do
    for j in 1 2 4 8 16 32 64 128; do
        ./main $i $j 1
    done
done
