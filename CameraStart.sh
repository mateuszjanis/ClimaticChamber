#!/bin/bash

FOLDER="Images"
IMAGE_INTERVAL=300
 
echo "Rozpoczęcie wykonywania zdjęć"
echo "-----------------------------"

while true; do

	PLIK="${FOLDER}/image_$(date +%Y%m%d_%H%M%S).jpg"

	echo "[$(date +%T)] Wykonanie zdjęcia: $PLIK"

	pinctrl set 16 dl

	sleep 1

	rpicam-still -n --output "$PLIK" > /dev/null 2>&1 

	sleep 1
	
	pinctrl set 16 dh

	sleep $IMAGE_INTERVAL
done


