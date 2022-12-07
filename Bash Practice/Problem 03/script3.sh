for file in *[0-9]*; do
    if [[ $file != $0 ]] && [[ -f $file ]]; then
        # echo "$file"
        rm "$file"
    fi
done