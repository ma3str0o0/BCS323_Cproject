#!/bin/bash

echo "[Monitor] Watching system resource usage..."

watch -n 2 '
    echo "Timestamp: $(date)"
    echo "----------------------------------------"
    ps -eo pid,comm,%cpu,%mem --sort=-%cpu | grep -E "master|worker"
'
