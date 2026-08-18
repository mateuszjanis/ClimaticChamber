#!/bin/bash

FOLDER="$HOME/Documents/images"
 
echo "Rozpoczęcie wykonywania zdjęć"
echo "-----------------------------"

while true; do

	PLIK="${FOLDER}/image_$(date +%Y%m%d_%H%M%S).jpg"

	echo "[$(date +%T)] Wykonanie zdjęcia: $PLIK"

	rpicam-still --output "$PLIK" 

	sleep 10
done


