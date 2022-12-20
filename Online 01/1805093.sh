declare -A fd
declare -A freefd

# read -r n < input.txt
# echo "$n"

lastmaxfd=0

while read -r line; do

    first=${line%% *}
    last=${line##* }
    # echo "$first"
    if [[ $first == "open" ]]; then
        echo "eta open"
        # flag=0
        ((lastmaxfd=lastmaxfd+1))
        for i in $(seq 0 $lastmaxfd); do 
            if [[ ! -v fd[$i] ]]; then
                [[ $lastmaxfd<$i ]] && $lastmaxfd=$i
                fd[$i]=${last%.txt}
                # echo "$i"
                # flag=1
                touch ${last}
            fi
        done
        # if [[ $flag -eq 0 ]]; then
        #     ((lastmaxfd++))
        #     ((fd[$lastmaxfd]=${last%.txt}))
        # fi
        # echo "$lastmaxfd"
    elif [[ $first == "close" ]]; then
        echo "eta close"

        found=-1
        for key in "${!fd[@]}"; do
            if [[ ${fd[$key]} == "$last" ]]; then
                found=$key
            fi
        done
        ((fd= ${fd[@]/$found} ))
    elif [[ $first == "append" ]]; then
        echo "eta append"
    fi
done <input.txt

for key in ${!fd[@]}; do
    echo "$key,${fd[$key]}"
done