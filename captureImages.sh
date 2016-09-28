#!/bin/bash
#NAME   : captureImages.sh
#DESC   : Webcam image capture script. The ./SotreImages directory must exist and has writeable to save captured images. The fswebcam tool requires to take an image. sudo apt-get install fswebcam to install this packgage.
#VERS   :
#   kwangEun An - 2016.08.11 - Take an image from a webcam device.
#   kwangEun An - 2016.09.28 - Re-take an image if it has less than a size of 50KB.

#Configurations

file_raw="./StoreImages/$(date "+%d%m%y_%H%M").jpg" #An image file is named with sequence of day, month,year,hour and minute.

#Capture program configuration
#capture_device=" -d /dev/video0" #without this option, /dev/video0 is given as default device.
#capture_resolution=" -r 1280x720" #Without this option, default is 384x288
#capture_brightness=" -s Brightness=50%"
#capture_contrast=" -s Contrast=100%"

capture_raw="fswebcam -d /dev/video0 -r 1280x720 $file_raw"
#Get a current hour and time
hourmin="$(date "+%H%M")"

#Start capturing
if [ \( "$hourmin" -le 1930 \) -a \( "$hourmin" -ge 530 \) ]
then
    $capture_raw
    fileSize=$(wc -c < "$file_raw") 
    while [ $fileSize -le 50000 ] #if the file size is less than 50KB re-capture the image
    do
        echo "capture failed, doing it again."
        $capture_raw
        fileSize=$(wc -c < "$file_raw")
    done
fi
