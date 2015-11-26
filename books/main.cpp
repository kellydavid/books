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
#include "utilities.hpp"

using namespace std;
using namespace cv;

#define DEBUG_MODE

#define TOP_CORNER 0
#define LEFT_CORNER 1
#define RIGHT_CORNER 2
#define BOTTOM_CORNER 3

int bookview_actual_pages[RECOGNITION_COUNT] = {1,2,3,4,5,6,7,8,9,10,11,12,13,2,3,5,4,7,9,8,7,11,13,12,2};
Mat bookview_images[NUM_BOOKVIEW];
Mat page_images[NUM_PAGES];
Mat blue_colour_sample;

Point2f * get_corners(Point2f points[], int num_points);

void clone_point(Point2f source, Point2f *dest);

// returns the center point from a contour
Point2f get_centre(vector<Point> contour);

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
    
    vector<vector<Point2f>> corners(NUM_BOOKVIEW);
    // get contours
    for(int i = 0; i < NUM_BOOKVIEW; i++){
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(binary[i], contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
        Mat contours_image = binary[i].clone();
        cout << "Image #" << to_string(i) << " number contours #" << contours.size() << endl;
        Point2f *points = new Point2f[contours.size()];
        for(int j = 0; j < contours.size(); j++){
            points[j] = get_centre(contours[j]);
            cout << "Contour #" << j << " Centre Point: " << points[j].x << ", " << points[j].y << endl;
            Scalar colour(rand()&0xFF, rand()&0xFF, rand()&0xFF);
            drawContours(contours_image, contours, j, colour);
        }
        Point2f *corner = get_corners(points, (int)contours.size());
        corners[i].push_back(corner[3]);
        corners[i].push_back(corner[2]);
        corners[i].push_back(corner[1]);
        corners[i].push_back(corner[0]);
        cout << "Image #" << i << " (" << corner[0].x << ", " << corner[0].y << ") ("
        << corner[1].x << ", " << corner[1].y << ") (" << corner[2].x << ", " << corner[2].y << ") ("
        << corner[3].x << ", " << corner[3].y << ")" << endl;
        resize(contours_image, contours_image, Size(contours_image.cols / 2, contours_image.rows / 2));
//        DisplayImage(contours_image, "Contours " + to_string(i), 100, 100);
//        waitKey(0);
//        cvDestroyAllWindows();
    }
    
    
    // geometric transformation
    Mat *geo_transform = new Mat[NUM_BOOKVIEW]();
    for(int i = 0; i < NUM_BOOKVIEW; i++){
        Point2f source[4] = {*new Point2f(), *new Point2f(), *new Point2f(), *new Point2f()};
        if(corners[i][LEFT_CORNER].y < corners[i][RIGHT_CORNER].y){
            // if left corner higher than right corner
            // top left is left corner other wise its top corner
            clone_point(corners[i][TOP_CORNER], &source[0]);
            clone_point(corners[i][RIGHT_CORNER], &source[1]);
            clone_point(corners[i][LEFT_CORNER], &source[2]);
            clone_point(corners[i][BOTTOM_CORNER], &source[3]);
            
        }else{
            clone_point(corners[i][LEFT_CORNER], &source[0]);
            clone_point(corners[i][TOP_CORNER], &source[1]);
            clone_point(corners[i][BOTTOM_CORNER], &source[2]);
            clone_point(corners[i][RIGHT_CORNER], &source[3]);
        }
        Point2f dest[4];
        dest[0] = *new Point(8, 7); // top left
        dest[1] = *new Point(398, 7); // top right
        dest[2] = *new Point(8, 589); // bottom left
        dest[3] = *new Point(398, 589); // bottom right
        
        Mat perspective_transform = getPerspectiveTransform(source, dest);
        warpPerspective(bookview_images[i], geo_transform[i], perspective_transform, *new Size(400, 600));
        DisplayImage(geo_transform[i], "Geometric Transform " + to_string(i), 100, 100);
        waitKey(0);
        cvDestroyAllWindows();
    }
    
    return 0;
}

Point2f * get_corners(Point2f points[], int num_points){
    if(num_points > 4){
        Point2f *corners = new Point2f[4]();
        clone_point(points[0], &corners[0]);
        clone_point(points[0], &corners[1]);
        clone_point(points[0], &corners[2]);
        clone_point(points[0], &corners[3]);
        for(int i = 0; i < num_points; i++){
            // top
            if(points[i].y < corners[TOP_CORNER].y){
                clone_point(points[i], &corners[TOP_CORNER]);
            }
            // left
            if(points[i].x < corners[LEFT_CORNER].x){
                clone_point(points[i], &corners[LEFT_CORNER]);
            }
            // right
            if(points[i].x > corners[RIGHT_CORNER].x){
                clone_point(points[i], &corners[RIGHT_CORNER]);
            }
            // bottom
            if(points[i].y > corners[BOTTOM_CORNER].y){
                clone_point(points[i], &corners[BOTTOM_CORNER]);
            }
        }
        return corners;
    }
    else{
        return NULL;
    }
}

void clone_point(Point2f source, Point2f *dest){
    dest->x = source.x;
    dest->y = source.y;
}

Point2f get_centre(vector<Point> contour){
    int sumX = 0, sumY = 0;
    int size = (int)contour.size();
    for(int i = 0; i < size; i++){
        sumX += contour[i].x;
        sumY += contour[i].y;
    }
    return Point2f(sumX / size, sumY / size);
}

