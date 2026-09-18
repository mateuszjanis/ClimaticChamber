#!/bin/bash

nmcli connection up hotspot
sleep 10

sudo openvpn --config /home/pi/Downloads/VPN-AGH.2026.ovpn --daemon --log /dev/null
sleep 10

#mkdir Images
#mkdir Data
#sleep 5

./CameraStart.sh &

cd ChamberControl
./ChamberStart.sh &