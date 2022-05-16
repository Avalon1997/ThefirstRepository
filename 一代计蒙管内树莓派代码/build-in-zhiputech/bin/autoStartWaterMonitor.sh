#!/bin/bash
echo "auto start water monitor"
cd /home/pi/WaterMonitor/
sudo su  << EOF
nohup ./water-monitor-daq 1>/dev/null 2>nohub.out &
EOF




