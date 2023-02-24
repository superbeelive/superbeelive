#!/usr/bin/env bash

ffmpeg -f rawvideo -vcodec rawvideo -pix_fmt gray -s 1920x1080 -r 30 -i $1 \
	-c:v libx264 -pix_fmt gray -crf 0 -preset:v slow -c:a copy output.mkv

ffmpeg -i output.mkv -vcodec rawvideo -pix_fmt gray -f rawvideo output.raw
