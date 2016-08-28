#!/bin/bash

#Written by KwangEun

#Configurations

#File configuration
#fileName=$(date "+%d%m%y_%H%M").jpg
#fileLocation="StoreImages/"
#file=$fileLocation$fileName
file_raw="/home/pi/SmartPlanting/StoreImages/$(date "+%d%m%y_%H%M").jpg"
#Capture program configuration
#capture_device=" -d /dev/video0" #without this option, /dev/video0 is given as default device.
#capture_resolution=" -r 1280x720" #Without this option, default is 384x288
#capture_brightness=" -s Brightness=50%"
#capture_contrast=" -s Contrast=100%"
#Camera without banner version
#capture="fswebcam"$capture_resolution$capture_brightness" --no-banner "$file""

#Camera with banner version
#capture="fswebcam"$capture_resolution$capture_brightness" "$file
capture_raw="fswebcam -d /dev/video0 -r 1280x720 $file_raw"
#Get a current hour and time
hourmin="$(date "+%H%M")"

#Start capturing
if [ \( "$hourmin" -le 1930 \) -a \( "$hourmin" -ge 530 \) ]
then
    $capture_raw
    fileSize=$(wc -c < "$file_raw") 
    while [ $fileSize -le 50000 ]
    do
        echo "capture failed, doing it again."
        $capture_raw
        fileSize=$(wc -c < "$file_raw")
    done
fi
