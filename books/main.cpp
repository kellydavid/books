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

#define PATH_TO_IMAGES "../../books/images"
#define RECOGNITION_COUNT 25 // number of images to recognise from sample
#define NUM_BOOKVIEW 50 // total number of images in sample
#define NUM_PAGES 13 // the number of images of the actual pages
#define BLUE_COLOUR_SAMPLE "blue_sample.png"
#define BOOKVIEW "BookView"
#define ACTUAL_PAGE "Page"
#define JPEG_EXTENSION ".jpg"

int bookview_actual_pages[RECOGNITION_COUNT] = {1,2,3,4,5,6,7,8,9,10,11,12,13,2,3,5,4,7,9,8,7,11,13,12,2};

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
