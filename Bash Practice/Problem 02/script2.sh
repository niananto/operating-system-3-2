for file in *; do
    # echo "$file"
    if [[ $file != $0 ]] && grep -q "$1" "$file"; then
        # echo "$file"
        rm "$file"
    fi
done