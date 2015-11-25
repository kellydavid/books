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

Point * get_corners(Point points[], int num_points);

void clone_point(Point source, Point *dest);

// returns the center point from a contour
Point get_centre(vector<Point> contour);

void DisplayImage(Mat image, string message, int x, int y);

bool load_image(string filename, Mat *image);

string get_bookview_path(int index);

string get_page_path(int index);

string get_image_path(string image);

int main(int argc, const char * argv[]) {
    cout << "Loading Images\n";
    // load all bookview images
    for(int i = 0; i < NUM_BOOKVIEW; i++){
        if(!load_image(get_bookview_path(i+1), &bookview_images[i])){
            return -1;
        }
    }
    // load all page images
    for(int i = 0; i < NUM_PAGES; i++){
        if(!load_image(get_page_path(i+1), &page_images[i])){
            return -1;
        }
    }
    // load blue colour sample image
    if(!(load_image(get_image_path(BLUE_COLOUR_SAMPLE), &blue_colour_sample))){
        return -1;
    }
    cout << "Successfully Loaded Images\n";
    
    // Back Projection
    ColourHistogram blue_hist = * new ColourHistogram(blue_colour_sample, 5);
    blue_hist.NormaliseHistogram();
    Mat *back_project = new Mat[NUM_BOOKVIEW]();
    for(int i = 0; i < NUM_BOOKVIEW; i++){
        back_project[i] = blue_hist.BackProject(bookview_images[i]);
//        DisplayImage(back_project[i], "back projection " + to_string(i), 100, 100);
//        waitKey(0);
//        cvDestroyAllWindows();
    }
    
    // Convert to binary
    Mat *binary = new Mat[NUM_BOOKVIEW]();
    Mat five_by_five_element(5, 5, CV_8U, Scalar(1));
    for(int i = 0; i < NUM_BOOKVIEW; i++){
        threshold(back_project[i], binary[i], 0, 255, THRESH_BINARY | THRESH_OTSU);
        morphologyEx(binary[i], binary[i], MORPH_CLOSE, five_by_five_element);
        Mat display = binary[i].clone();
        resize(display, display, Size(binary[i].cols/2, binary[i].rows/2));
//        DisplayImage(display, "Binary back projection " + to_string(i), 100, 100);
//        waitKey(0);
//        cvDestroyAllWindows();
    }
    
    // get contours
    for(int i = 0; i < NUM_BOOKVIEW; i++){
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(binary[i], contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
        Mat contours_image = binary[i].clone();
        cout << "Image #" << to_string(i) << " number contours #" << contours.size() << endl;
        Point *points = new Point[contours.size()];
        for(int j = 0; j < contours.size(); j++){
            points[j] = get_centre(contours[j]);
            cout << "Contour #" << j << " Centre Point: " << points[j].x << ", " << points[j].y << endl;
            Scalar colour(rand()&0xFF, rand()&0xFF, rand()&0xFF);
            drawContours(contours_image, contours, j, colour);
        }
        Point *corners = get_corners(points, (int)contours.size());
        cout << "Image #" << i << " (" << corners[0].x << ", " << corners[0].y << ") ("
        << corners[1].x << ", " << corners[1].y << ") (" << corners[2].x << ", " << corners[2].y << ") ("
        << corners[3].x << ", " << corners[3].y << ")" << endl;
        resize(contours_image, contours_image, Size(contours_image.cols / 2, contours_image.rows / 2));
        DisplayImage(contours_image, "Contours " + to_string(i), 100, 100);
        waitKey(0);
        cvDestroyAllWindows();
    }
    
    
    
    return 0;
}

Point * get_corners(Point points[], int num_points){
    if(num_points > 4){
        Point *corners = new Point[4]();
        clone_point(points[0], &corners[0]);
        clone_point(points[0], &corners[1]);
        clone_point(points[0], &corners[2]);
        clone_point(points[0], &corners[3]);
        for(int i = 0; i < num_points; i++){
            // right
            if(points[i].x > corners[2].x){
                clone_point(points[i], &corners[2]);
            }
            // bottom
            if(points[i].y < corners[3].y){
                clone_point(points[i], &corners[3]);
            }
            // top
            if(points[i].y > corners[0].y){
                clone_point(points[i], &corners[0]);
            }
            // left
            if(points[i].x < corners[1].x){
                clone_point(points[i], &corners[1]);
            }
        }
        return corners;
    }
    else{
        return NULL;
    }
}

void clone_point(Point source, Point *dest){
    dest->x = source.x;
    dest->y = source.y;
}

Point get_centre(vector<Point> contour){
    int sumX = 0, sumY = 0;
    int size = (int)contour.size();
    for(int i = 0; i < size; i++){
        sumX += contour[i].x;
        sumY += contour[i].y;
    }
    return Point(sumX / size, sumY / size);
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