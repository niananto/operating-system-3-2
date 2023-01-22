#!/bin/bash

# traverse all cpp files in current directory and subdirectory using *
for file in {,**/}*.cpp; do
    # rename .cpp to .c
    # see @ https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html
    mv "$file" "${file%.cpp}.c"
done
    