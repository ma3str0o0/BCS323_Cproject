#!/bin/bash

BASE_DIR="./data"
NUM_DIRS=4    # Number of directories
FILES_PER_DIR=5  # Files per directory

echo "Generating test directories and files..."

mkdir -p "$BASE_DIR"

for d in $(seq 1 $NUM_DIRS); do
    DIR="$BASE_DIR/dir$d"
    mkdir -p "$DIR"
    for f in $(seq 1 $FILES_PER_DIR); do
        FILE="$DIR/file$f.txt"
        echo "Generating $FILE"
        for l in $(seq 1 20); do
            echo "This is line $l in $FILE with some searchable words like apple, banana, or cherry." >> "$FILE"
        done
    done
done

echo "Test files generated."

