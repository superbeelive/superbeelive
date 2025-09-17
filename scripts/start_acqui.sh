#!/bin/bash

set -euo pipefail

if [ "$#" -ne 2 ]; then
	echo "Usage: $0 <module> <camera." >&2
	exit 1
fi

module="$1"
cam="$2"
ip=$(( 98 + 2*(module) + cam ))
screen_name="M${module}C${cam}"

screen -dmS $screen_name bash -c "sbl_cam_daemon \
	-d 1 -w 1920 -h 1200 -f 30 -o /beegfs/superbeelive_data/ \
	-m $module -c $cam 10.24.3.$ip"
