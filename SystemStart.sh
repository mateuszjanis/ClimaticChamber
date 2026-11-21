#!/bin/bash

nmcli connection up hotspot_chamber

sleep 15

sudo openvpn --config /home/pi/Downloads/VPN-AGH.2027.ovpn --daemon --log /dev/null

sleep 15

cd ChamberControl
./ChamberStart.sh