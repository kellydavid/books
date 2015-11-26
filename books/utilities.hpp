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
#define BOOKVIEW "BookView"
#define ACTUAL_PAGE "Page"
#define JPEG_EXTENSION ".jpg"

#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

void DisplayImage(cv::Mat image, std::string message, int x, int y);

bool load_image(std::string filename, cv::Mat *image);

std::string get_bookview_path(int index);

std::string get_page_path(int index);

std::string get_image_path(std::string image);


#endif /* utilities_hpp */
