# ImageMarker

## Description
This script is used to label the position of one type of object in images. The labels can be used for training object detection models.

## System Requirements
- OpenCV 2.x
- Linux (Ubuntu 14.04 preferred)

## Format of Output
Each image has one corresponding output text file, which has the same name as the image. Each line in the text file has the same format: *"[label] [center_x] [center_y] [width] [height]"* The last four parameters represnts the bounding box in the image and all their values are expressed in the ratio to image width/height.

## How to Use
Use command line: *"./marker [tag] [image_dir]"*. If you make some changes to the src code, remove 'CMakeCache.txt' then in command line *"cmake ."* and *"make"*.

The script will open jpg or png files one by one, use your mouse to drag a bounding box on the targe object. You can draw multiple times if there are more than one. Press SPACE to go to the next image. 

An example of ImageMarker's applications is [YOLO object detector](https://github.com/hyperchris/Yolo)
