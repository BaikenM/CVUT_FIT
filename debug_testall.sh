#!/bin/bash
# A modified version of a simple bash script, https://courses.fit.cvut.cz/BI-PA1/media/videos/03-test.mkv

# Path to the source file (.c) 
SRC=$1
# Path to sample files (.txt)
SAMPLE=$2
# Program name after compiling
PROG=${SRC::-2}

# Compile for PROGTEST
g++ -std=c++14 -Wall -pedantic -Wno-long-long -O2 -Werror -o $PROG $SRC

if [ -z "$PROG" ] || [ -z "$SAMPLE" ]; then
    echo "Usage: ./testall.sh [program_path/] [sample_path/]"
    exit 1
fi

# Get input files as IN_FILE
# Get output files to compare to my_out.txt as REF_FILE
for IN_FILE in $2*_in.txt ; do
    REF_FILE=`echo -n $IN_FILE | sed -e 's/\([0-9]\)_in.txt$/\1_out.txt/'`
    ./$PROG < $IN_FILE > my_out.txt
    if ! diff $REF_FILE my_out.txt ; then
        echo "Fail: $IN_FILE";
        rm my_out.txt
	exit
    else
        echo "OK:  $IN_FILE";
    fi
done

# Clean up
rm $PROG
rm my_out.txt
