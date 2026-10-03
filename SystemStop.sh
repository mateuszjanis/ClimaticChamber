#!/bin/bash

echo "Stopping proceses ..."

pkill -f "CameraStart.sh"
pkill -f "ChamberStart.sh"
pkill -f "SystemStart.sh"

pkill -f "ChamberControl"

pkill -f "rpicam-still"

pinctrl set 17 dh
pinctrl set 23 dh
pinctrl set 24 dh
pinctrl set 27 dh

pinctrl set 16 dh
pinctrl set 25 dh

echo "Processes stopped!"