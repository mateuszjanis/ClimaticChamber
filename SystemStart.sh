#!/bin/bash

nmcli connection up chamberAP

sleep 10

sudo openvpn --config /home/pi/Downloads/VPN-AGH.2027.ovpn --daemon --log /dev/null

sleep 10

cd ChamberControl
./ChamberStart.sh