//
//  main.cpp
//  books
//
//  Created by David Kelly on 23/11/2015.
//  Copyright © 2015 David Kelly. All rights reserved.
//

#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "histogram.hpp"

using namespace std;
using namespace cv;

#define DEBUG_MODE
#define PATH_TO_IMAGES "../../books/images/"
#define RECOGNITION_COUNT 25 // number of images to recognise from sample
#define NUM_BOOKVIEW 50 // total number of images in sample
#define NUM_PAGES 13 // the number of images of the actual pages
#define BLUE_COLOUR_SAMPLE "blue_sample.png"
#define BOOKVIEW "BookView"
#define ACTUAL_PAGE "Page"
#define JPEG_EXTENSION ".jpg"

int bookview_actual_pages[RECOGNITION_COUNT] = {1,2,3,4,5,6,7,8,9,10,11,12,13,2,3,5,4,7,9,8,7,11,13,12,2};
Mat bookview_images[NUM_BOOKVIEW];
Mat page_images[NUM_PAGES];
Mat blue_colour_sample;

void DisplayImage(Mat image, string message, int x, int y);

bool load_image(string filename, Mat *image);

string get_bookview_path(int index);

string get_page_path(int index);

string get_image_path(string image);

int main(int argc, const char * argv[]) {
    cout << "Loading Images\n";
    // load all bookview images
    for(int i = 1; i <= NUM_BOOKVIEW; i++){
        if(!load_image(get_bookview_path(i), &bookview_images[i])){
            return -1;
        }
    }
    // load all page images
    for(int i = 1; i <= NUM_PAGES; i++){
        if(!load_image(get_page_path(i), &page_images[i])){
            return -1;
        }
    }
    // load blue colour sample image
    if(!(load_image(get_image_path(BLUE_COLOUR_SAMPLE), &blue_colour_sample))){
        return -1;
    }
    cout << "Successfully Loaded Images\n";
    return 0;
}

// Displays a single image
void DisplayImage(Mat image, string message, int x, int y){
    namedWindow(message, CV_WINDOW_NORMAL);
    moveWindow(message, x, y);
    imshow(message, image);
}

bool load_image(string filename, Mat *image){
    *image = imread(filename, 1);
    if(image->empty()){
        cout << "Could not open " << filename << "." << endl;
        return false;
    }else{
        return true;
    }
}

string get_bookview_path(int index){
    ostringstream os;
    os << PATH_TO_IMAGES << BOOKVIEW << (index <= 9 ? "0":"") << index << JPEG_EXTENSION;
    return os.str();
}

string get_page_path(int index){
    ostringstream os;
    os << PATH_TO_IMAGES << ACTUAL_PAGE << (index <= 9 ? "0":"") << index << JPEG_EXTENSION;
    return os.str();
}

string get_image_path(string image){
    ostringstream os;
    os << PATH_TO_IMAGES << image;
    return os.str();
}