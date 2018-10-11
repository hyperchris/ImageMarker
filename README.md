# ImageMarker

## Description
This script is used to label the position of one type of object in images. The labels can be used for training object detection models.

## System Requirements
- OpenCV
- Linux (Ubuntu 14.04 preferred)

## Format of Output
Each image has one corresponding output text file in ```pos/``` folder, which has the same name as the image. Each line in the text file has the same format: *"[label] [center_x] [center_y] [width] [height]"* The last four parameters represnts the bounding box in the image and all their values are expressed in the ratio to image width/height.

## How to Use
First, go to the root dir of ImageMarker. Then run ```cmake .``` and ```make all```.

Then, use command line: *"./marker [objet_label] [image_dir]"*. The label can only be *integer*. If you make some changes to the src code, remove 'CMakeCache.txt' then in command line *"cmake ."* and *"make"*. If you want to label different objects in the images, you will need to run the script multiple times with different object labels. Note that the results for the single image will be *appended* in the file. 

The script will open jpg or png files one by one, use your mouse to drag a bounding box on the targe object. You can draw multiple times if there are more than one. Press SPACE or ESC to go to the next image. 
