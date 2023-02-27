#!/bin/bash
# script takes 4 arguments that are given to the master worker program

gcc -o master-worker master-worker.c -lpthread
./master-worker $1 $2 $3 $4 > output.txt
awk -f check.awk MAX=$1 output.txt

rm master-worker