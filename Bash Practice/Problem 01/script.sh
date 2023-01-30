if [[ $# = 0 ]] ; then
    echo "Usage $0 filename1 filename2 ..."
else 

    for var in "$@"
    do
        # echo "$var"
        if [[ -d $var ]]; then
            echo "$var is not a regular file"
        elif [[ -f $var ]]; then
            # echo "$var is a file"
            if [[ -x $var ]]; then
                echo "$var is currently executable"
                ls -l "$var"
                echo "$var's execution permission is now changing"
                chmod -x "$var"
                ls -l "$var"
                echo "$var is currently not executable"

            else
                echo "$var is currently not executable"
            fi

        else
            echo "$var does not exist"
            # exit 1
        fi

    done

fi