#!/bin/bash

echo "[Shutdown] Shutting down system..."

# Send shutdown signals (using pkill for simplicity)
pkill master
pkill worker

echo "[Shutdown] All processes terminated."
