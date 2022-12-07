#!/bin/bash

# run failrand.sh till it exits 1, capture its stdout and stderr and print them
while true; do
    if ! output=$(./failrand.sh 2>&1); then
        echo "Something went wrong"
        echo "$output"
        exit 1
    fi
done