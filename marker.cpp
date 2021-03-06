/*
 * Function: Store the position of ground truth object in given images
 * Input: ./img_marker [int_tag] [img_path_1] .. [img_path_n]
 * Output: Multiple files in a folder. Each file's name is for one of the images. 
          The content line is '[tag] [x] [y] [width] [height]'
          Note that x and y represents the CENTER point of the box
          All numbers are expressed in ratio to the image width/height 
 */

// 
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <cctype>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>

using namespace std;
using namespace cv;

#define RES_DIR "pos/" // this dir must exist together w/ the compiled program
#define MIN_SIZE 10 // if the box is smaller than this size, ignore.

Mat org; // image read in  
Mat img; // image to show
Mat tmp; // temporary image for displaying the box

static int tag; // label of the object
static string position_file; // position data will be stored here
static string img_dir;  // dir that stores the test images

int getDir (string dir, vector<string> &files) { // get files in a dir
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

// append a line to a text file with / endl
void appendFile(string msg, string path) {
  ofstream outfile; // output file handler
  outfile.open(path, ofstream::app);
  outfile << msg << endl;
  outfile.close();
}

void on_mouse(int event,int x,int y,int flags,void *ustc) { 
  static Point pre_pt = Point(-1,-1); // initial position
  static Point cur_pt = Point(-1,-1); // mouse position

  char temp[16];
  if (event == CV_EVENT_LBUTTONDOWN) {// Press down the left key
    org.copyTo(img);
    sprintf(temp,"(%d,%d)",x,y);
    pre_pt = Point(x,y);
    putText(img,temp,pre_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255),1,8);// show the coordinate
    circle(img,pre_pt,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);// draw the initial point that the mouse pressed
    imshow("img",img);
  }
  else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON)) {// Mouse move w/o key pressed
    img.copyTo(tmp);
    sprintf(temp,"(%d,%d)",x,y);
    cur_pt = Point(x,y);
    putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));
    imshow("img",tmp);
  }
  else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)) { // Draw the box while moving the mouse
    img.copyTo(tmp);
    sprintf(temp,"(%d,%d)",x,y);
    int cur_x = (x >= img.cols) ? img.cols - 1 : x;
    int cur_y = (y >= img.rows) ? img.rows - 1 : y;
    cur_pt = Point(cur_x,cur_y);
    putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));
    rectangle(tmp,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);
    imshow("img",tmp);
  }
  else if (event == CV_EVENT_LBUTTONUP) { // show the box after depressing left key
    org.copyTo(img);
    sprintf(temp,"(%d,%d)",x,y);
    int cur_x = (x >= img.cols) ? img.cols - 1 : x;
    int cur_y = (y >= img.rows) ? img.rows - 1 : y;
    cur_pt = Point(cur_x,cur_y);
    putText(img,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));
    circle(img,pre_pt,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);
    rectangle(img,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0); 
    imshow("img",img);
    img.copyTo(tmp);
    // decide if the box is too small
    int width = abs(pre_pt.x - cur_pt.x); // get the wid/hei of cropped area
    int height = abs(pre_pt.y - cur_pt.y);
    if (width < MIN_SIZE || height < MIN_SIZE) return;
    // store a line to position file
    string line = to_string(tag) + 
              " " + to_string(((float)pre_pt.x + (float)width / 2) / (float)img.cols) +  // center point of the box
              " " + to_string(((float)pre_pt.y + (float)height / 2) / (float)img.rows) + 
              " " + to_string((float)width / (float)img.cols) + 
              " " + to_string((float)height / (float)img.rows);
    appendFile(line, position_file);
  }
}

int main(int argc, char** argv) {
  // input format: ./marker [image_path] [win_width] [win_height]
  if (argc != 3) {
    cout << "ERROR: wrong argc! Should be: ./marker [tag] [image_path]" << endl;
    return -1;
  }

  tag = stoi(argv[1]);
  img_dir = string(argv[2]);
  if (img_dir.back() != '/')
    img_dir += "/";
  cout << "tag: " << tag << endl;
  cout << "image dir: " << img_dir << endl;
  vector<string> files = vector<string>();
  
  getDir(img_dir,files);
  
  for (int i = 0; i < files.size(); i++) {
    if (files[i].find(".jpg") == std::string::npos && files[i].find(".png") == std::string::npos)  // skip if the file is not an image
      continue;
    string img_name = files[i];
    cout << "Image: " << img_name << endl;
    string img_path = img_dir + files[i];
    org = imread(img_path); // read in original image
    org.copyTo(img);
    org.copyTo(tmp);
    position_file = string(RES_DIR) + img_name.substr(0, img_name.find_last_of(".")) + string(".txt"); // the position files
    
    ofstream outfile; // create position file for the image (Yolo requires every image w/ a pos file)
    outfile.open(position_file, ofstream::app);
    outfile.close();

    namedWindow("img"); // show the original image
    setMouseCallback("img", on_mouse, 0); // main function
    imshow("img",img);
    waitKey(0);
  }

  return 0;
}
