//
//  utilities.hpp
//  books
//
//  Created by David Kelly on 26/11/2015.
//  Copyright © 2015 David Kelly. All rights reserved.
//

#ifndef utilities_hpp
#define utilities_hpp

#define PATH_TO_IMAGES "../../books/images/"
#define RECOGNITION_COUNT 25 // number of images to recognise from sample
#define NUM_BOOKVIEW 50 // total number of images in sample
#define NUM_PAGES 13 // the number of images of the actual pages
#define BLUE_COLOUR_SAMPLE "blue_sample.png"
#define NUM_BINS_BLUE_BACK_PROJECT 5
#define LENGTH_ROI_BLUE_POINTS 30
#define BOOKVIEW "BookView"
#define ACTUAL_PAGE "Page"
#define JPEG_EXTENSION ".jpg"

#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

/******* Image operation helper functions *******/

// returns binary image
void otsu_threshold(cv::Mat *image);

// can be used on a binary image
void binary_closing_operation(cv::Mat *image);

// back projects the colour sample
cv::Mat back_project(cv::Mat image, cv::Mat colour_sample, int number_bins);

/******* Image Display Functions ******************/

void DisplayImage(cv::Mat image, std::string message, int x, int y);

void draw_points(cv::Mat *image, std::vector<cv::Point2f> points);

bool load_image(std::string filename, cv::Mat *image);

/********* File and Path functions ********************/

std::string get_bookview_path(int index);

std::string get_page_path(int index);

std::string get_image_path(std::string image);


#endif /* utilities_hpp */
