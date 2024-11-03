#!/bin/bash
cd /home/hich/smarthome
killall node
node cameraserver.js &
