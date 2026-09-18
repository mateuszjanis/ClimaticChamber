#!/bin/bash

FOLDER="Images"
IMAGE_INTERVAL=60
 
echo "Rozpoczęcie wykonywania zdjęć"
echo "-----------------------------"

while true; do

	PLIK="${FOLDER}/image_$(date +%Y%m%d_%H%M%S).jpg"

	echo "[$(date +%T)] Wykonanie zdjęcia: $PLIK"

	rpicam-still -n --output "$PLIK" > /dev/null 2>&1 

	sleep $IMAGE_INTERVAL
done


