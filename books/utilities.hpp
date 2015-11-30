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
#define NUM_POINTS_ON_PAGE 20 // number of points on page
#define BLUE_COLOUR_SAMPLE "blue_sample.png"
#define NUM_BINS_BLUE_BACK_PROJECT 5
#define LENGTH_ROI_BLUE_POINTS 30
#define RATIO_BLACK_PIXELS_ROI_BLUE_POINTS 3
#define BOOKVIEW "BookView"
#define ACTUAL_PAGE "Page"
#define JPEG_EXTENSION ".jpg"
#define TEMPLATE_SCALE 0.5
#define TEMPLATE_LEFT_CORNER_X 8
#define TEMPLATE_RIGHT_CORNER_X 398
#define TEMPLATE_TOP_CORNER_Y 7
#define TEMPLATE_BOTTOM_CORNER_Y 589
#define TEMPLATE_HEIGHT 620
#define TEMPLATE_WIDTH 420

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

cv::Mat rescaleImage(cv::Mat image, double scale);

cv::Mat JoinImagesHorizontally( cv::Mat& image1, std::string name1, cv::Mat& image2, std::string name2, int spacing, cv::Scalar passed_colour );

void writeText( cv::Mat image, std::string text, int row, int column, cv::Scalar passed_colour, double scale=0.5, int thickness=1.0 );

bool load_image(std::string filename, cv::Mat *image);

/********* File and Path functions ********************/

std::string get_bookview_path(int index);

std::string get_page_path(int index);

std::string get_image_path(std::string image);


#endif /* utilities_hpp */
