recurse() {
 for i in "$1"/*;do
    if [ -d "$i" ];then
        # echo "dir: $i"
        recurse "$i"
    elif [ -f "$i" ] && [[ $i == *.cpp ]]; then
        # echo "file: $i"
        mv "$i" "${i: 0:-2}"
    fi
 done
}

recurse .
