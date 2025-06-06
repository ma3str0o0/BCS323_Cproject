#!/bin/bash

Num_Workers=$(grep "num_workers" "config.txt" | cut -d'=' -f2)

echo "[Launch] Launching $NUM_WORKERS workers..."

for i in $(seq 1 $Num_Workers); do
    ./worker $i &
    echo "[Launch] Worker $i started with PID $!"
done

sleep 2

echo "[Launch] Launching master..."
./master &
echo "[Launch] Master started with PID $!"

echo "[Launch] System is up and running."
