#!/bin/bash

FILE_PATH=$1

echo "[$(date +%T)] Wykonanie zdjęcia: $FILE_PATH"

pinctrl set 16 dl
sleep 0.5
rpicam-still -n --output "$FILE_PATH" > /dev/null 2>&1 
sleep 0.5
pinctrl set 16 dh