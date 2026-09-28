#!/bin/bash

nmcli connection up hotspot

sudo openvpn --config /home/pi/Downloads/VPN-AGH.2026.ovpn --daemon --log /dev/null

sleep 5

./CameraStart.sh &

cd ChamberControl
./ChamberStart.sh &