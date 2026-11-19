#!/bin/bash

nmcli connection up hotspot_chamber

sudo openvpn --config /home/pi/Downloads/VPN-AGH.2027.ovpn --daemon --log /dev/null

sleep 5

cd ChamberControl
./ChamberStart.sh