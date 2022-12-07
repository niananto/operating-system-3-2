#!/bin/bash

# assigning variables
max_score=100
max_student_id=5
if [[ $# == 1 ]]; then
    max_score="$1"
elif [[ $# == 2 ]]; then
    max_score="$1"
    max_student_id="$2"
elif [[ $# > 2 ]]; then
    echo "please specify max_score max_student_id[optional] only"
    exit 1
fi

declare -A marks

# loop over the folder submissions
mkdir temp_outputs temp_codes
cd Submissions/
for dir in *; do
    # if [[ -f $dir ]]; then
    #     # echo "$dir is a file"
    #     # now to give this man zero
    #     marks[$(basename $dir .sh)]=0
    #     continue
    # fi

    # echo "$dir is a directory"
    a=$(ls -1 $dir | wc -l)
    if [[ $a == 0 ]]; then
        # zero marks
        # echo "No file inside the directory $dir"
        marks[$dir]=0
        continue
    elif [[ $a > 1 ]]; then
        # what to do?
        # echo "More than one file submitted in $dir"
        marks[$dir]=0
        continue
    fi

    b=$(ls -1 $dir/*.sh | wc -l )
    if [[ $b != 1 ]]; then
        # zero marks
        # echo "No .sh file in $dir"
        marks[$dir]=0
        continue        
    fi

    cd $dir
    c=$(ls -1 *.sh)
    if [[ ${c: 0:-3} != $dir ]]; then
        # zero marks
        # echo "Directory name $dir and file name $c don't match"
        marks[$dir]=0
        cd ..
        continue
    fi
    cd ..

    # storing output to a file
    d="${c: 0:-3}.txt"
    bash "$dir/$c" > ../temp_outputs/"$d"

    # marking based on accepted answer
    e=$(diff -w ../AcceptedOutput.txt ../temp_outputs/$d | grep [\<\>] | wc -l)
    ((score = max_score - e*5))
    ((score = score<0 ? 0 : score))
    # echo "$score"
    marks[$dir]=$score

    # copy-checker
    if [[ $(ls -1 ../temp_codes | wc -l) != 0 ]]; then
        for other_codes in ../temp_codes/*; do
            f=$(diff -ZB $dir/$c $other_codes | wc -l)
            g=$(basename $other_codes .sh)
            if [[ $f == 0 ]]; then
                ((marks[$dir] = marks[$dir] < -marks[$dir] ? marks[$dir] : -marks[$dir]))
                ((marks[$g] = marks[$g] < -marks[$g] ? marks[$g] : -marks[$g]))
                # echo "$dir and $g are same"
            fi
        done
    fi

    # storing these codes to a temp folder for copy checking
    cp "$dir/$c" ../temp_codes/"$c"

done
cd ..
rm -r temp_outputs temp_codes

((h=1805120+$max_student_id))
for i in $(seq 1805121 $h); do 
    if [[ ! -v marks[$i] ]]; then
        marks[$i]=0
    fi
done

echo "student_id,score" > output.csv
for roll in $(for x in ${!marks[@]}; do echo $x; done | sort); do
    echo "$roll,${marks[$roll]}" >> output.csv
done


