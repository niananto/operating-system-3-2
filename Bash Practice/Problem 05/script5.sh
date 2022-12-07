((x=0))
while true; do
    if bash failingScript.sh > out.txt 2> err.txt; then
        ((x++))
    else 
        break
    fi
done

echo "failed after $x attempts"