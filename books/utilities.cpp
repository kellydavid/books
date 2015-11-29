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

cv::Mat rescaleImage(cv::Mat image, double scale){
    Mat result;
    resize(image, result, Size(image.cols * scale, image.rows * scale));
    return result;
}

Mat JoinImagesHorizontally( Mat& image1, string name1, Mat& image2, string name2, int spacing, Scalar passed_colour){
    Mat result( (image1.rows > image2.rows) ? image1.rows : image2.rows,
               image1.cols + image2.cols + spacing,
               image1.type() );
    result.setTo(Scalar(255,255,255));
    Mat imageROI;
    imageROI = result(cv::Rect(0,0,image1.cols,image1.rows));
    image1.copyTo(imageROI);
    if (spacing > 0)
    {
        imageROI = result(cv::Rect(image1.cols,0,spacing,image1.rows));
        imageROI.setTo(Scalar(255,255,255));
    }
    imageROI = result(cv::Rect(image1.cols+spacing,0,image2.cols,image2.rows));
    image2.copyTo(imageROI);
    writeText( result, name1, 13, 6, passed_colour );
    writeText( imageROI, name2, 13, 6, passed_colour );
    return result;
}

void writeText( Mat image, string text, int row, int column, Scalar passed_colour, double scale, int thickness ){
    Scalar colour( 0, 0, 255);
    Point location( column, row );
    putText( image, text.c_str(), location, FONT_HERSHEY_SIMPLEX, scale, (passed_colour.val[0] == -1.0) ? colour : passed_colour, thickness );
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