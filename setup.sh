#!/bin/bash

# Setup Variables
Base="./data"
Num_Dir=5
Files_in_Dir=10
Lines_In_File=20

echo "[Setup] Generating test directories and files..."

mkdir -p "$Base"

for d in $(seq 1 $Num_Dir); do
    DIR="$Base/dir$d"
    mkdir -p "$DIR"
    for f in $(seq 1 $Files_in_Dir); do
        FILE="$DIR/file$f.txt"
        echo "Creating $FILE..."
        for l in $(seq 1 $Lines_in_File); do
            echo "Line $l: Random text with keywords like error, failure, network, disk." >> "$FILE"
        done
    done
done

echo "[Setup] Done creating test data."
