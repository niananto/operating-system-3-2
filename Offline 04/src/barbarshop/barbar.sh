#!/bin/bash
# script takes 4 arguments that are given to the master worker program

gcc -o barbarshop-a barbarshop-a.c -lpthread
./barbarshop-a

rm barbarshop-a