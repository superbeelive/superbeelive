#!/usr/bin/env bash

ffmpeg -f rawvideo -vcodec rawvideo -pix_fmt gray -s 1920x1080 -r 30 -i $1 \
	-c:v ffv1 -level 3 output.mkv

ffmpeg -i output.mkv -vcodec rawvideo -pix_fmt gray -f rawvideo output.raw
