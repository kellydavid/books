//
//  utilities.cpp
//  books
//
//  Created by David Kelly on 26/11/2015.
//  Copyright © 2015 David Kelly. All rights reserved.
//

#include "utilities.hpp"
#include "histogram.hpp"

using namespace cv;
using namespace std;


/******* Image operation helper functions *******/

void otsu_threshold(Mat *image){
    threshold(*image, *image, 0, 255, THRESH_BINARY | THRESH_OTSU);
}

void binary_closing_operation(Mat *image){
    Mat five_by_five_element(5, 5, CV_8U, Scalar(1));
    morphologyEx(*image, *image, MORPH_CLOSE, five_by_five_element);
}

Mat back_project(Mat image, Mat colour_sample, int number_bins){
    // Back Projection
    ColourHistogram blue_hist = * new ColourHistogram(colour_sample, number_bins);
    blue_hist.NormaliseHistogram();
    return blue_hist.BackProject(image);
}

/******* Image Display Functions ******************/

void DisplayImage(Mat image, string message, int x, int y){
    namedWindow(message, CV_WINDOW_NORMAL);
    moveWindow(message, x, y);
    imshow(message, image);
}

void draw_points(Mat *image, vector<Point2f> points){
    for(int i = 0; i < (int)points.size(); i++){
        circle(*image, points[i], 5, Scalar(0, 0, 255), -1);
    }
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

/********* File and Path functions ********************/

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