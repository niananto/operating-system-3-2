#!/bin/bash

recurse() {
    for i in "$1"/*;do
        if [ -d "$i" ];then
            echo "dir: $i"
            recurse "$i"
        elif [ -f "$i" ]; then
            echo "file: $i"
            # playername="${i##*/}"
            # playername="${playername%.txt}"
            # echo "Playername: $playername"
            lineno=0
            playername=""
            role=""
            country=""
            cat "$i" | while read -r line; do
                ((lineno=lineno+1))
                if [ $lineno -eq 1 ]; then
                    playername="$line"
                elif [ $lineno -eq 2 ]; then
                    country="$line"
                elif [ $lineno -eq 4 ]; then
                    role="$line"
                    mkdir "./temp/$country"
                    mkdir "./temp/$country/$role"
                    cp -f "$i" "./temp/${country}/${role}/${playername}.txt"
                    break
                fi
            done
        fi
    done
}

mkdir temp
recurse .
