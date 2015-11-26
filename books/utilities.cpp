//
//  utilities.cpp
//  books
//
//  Created by David Kelly on 26/11/2015.
//  Copyright © 2015 David Kelly. All rights reserved.
//

#include "utilities.hpp"

using namespace cv;
using namespace std;

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