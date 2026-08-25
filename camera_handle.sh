#!/bin/bash

FOLDER="$HOME/Documents/images"

sudo openvpn Downloads/VPN-AGH.2026.ovpn --daemon
lftp sftp://matjanis@student.agh.edu.pl

echo "Rozpoczęcie wykonywania zdjęć"
echo "-----------------------------"

while true; do

	PLIK="${FOLDER}/image_$(date +%Y%m%d_%H%M%S).jpg"

	echo "[$(date +%T)] Wykonanie zdjęcia: $PLIK"

	rpicam-still --output "$PLIK" 

	sleep 10
done


