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
        echo "this is open"
        ((lastmaxfd=lastmaxfd+1))
        for i in $(seq 0 $lastmaxfd); do 
            if [[ ! -v fd[$i] ]]; then
                [[ $lastmaxfd<$i ]] && $lastmaxfd=$i
                fd[$i]=${last%.txt}
                touch ${last}

                ######################
                [[ $i -ne $lastmaxfd ]] && ((lastmaxfd=lastmaxfd-1))
                ######################
            fi
        done

    elif [[ $first == "close" ]]; then
        echo "this is close"

        found=-1
        for key in "${!fd[@]}"; do
            if [[ ${fd[$key]} == "$last" ]]; then
                found=$key
            fi
        done
        ((fd= ${fd[@]/$found} ))
    elif [[ $first == "append" ]]; then
        echo "this is append"

        #################
        linesplit=(${line//;/ })
        middle=${linesplit[1]}   
        file=${fd[$middle]}
        echo "$last" >> "${file}.txt"
        ################
    fi
done <input.txt

for key in ${!fd[@]}; do
    echo "$key,${fd[$key]}"
done