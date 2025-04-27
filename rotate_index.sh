#!/bin/bash

TIMESTAMP=$(date +%Y%m%d_%H%M%S)

echo "[Backup] Backing up index data..."

mkdir -p "./backup"
cp -r "./index" "$./backup/index_backup_$TIMESTAMP"

echo "[Backup] Backup complete: ./backup/index_backup_$TIMESTAMP"
