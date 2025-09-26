#!/bin/bash

set -euo pipefail

screen -dmS websocket bash -c "/usr/bin/node /beegfs/superbeelive_data/web_server/html/video_extraction/server.js"
