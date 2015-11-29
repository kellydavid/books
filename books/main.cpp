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
#include "opencv2/xfeatures2d.hpp"
#include "histogram.hpp"
#include "utilities.hpp"

using namespace std;
using namespace cv;

#define TOP_LEFT_CORNER 0
#define TOP_RIGHT_CORNER 1
#define BOTTOM_LEFT_CORNER 2
#define BOTTOM_RIGHT_CORNER 3

//#define DISPLAY_PROCESSING
//#define DISPLAY_RESULT

// removes points that aren't on a white page
vector<Point2f> remove_outlier_points(Mat image, vector<Point2f> points);

// uses SIFT object recognition to return the best matching template index
int sift_match(Mat image, Mat *templates, int num_templates);

// performs template matching on the given image comparing it with a number of template images
int template_match(Mat image, Mat *templates, int num_templates);

// Applies the necessary geometric transformation to an image
// so that the page is extracted and correctly aligned
Mat geo_transform(Mat image, vector<Point2f> corners);

// finds the center of all the white areas in a binary image
vector<Point2f> get_points(Mat binary_image);

// gets the four corner points from an array of points
vector<Point2f> get_corners(vector<Point2f> points);

// returns the center point from a contour
Point2f get_centre(vector<Point> contour);

int main(int argc, const char * argv[]) {
    int known_truth[RECOGNITION_COUNT] = {1,2,3,4,5,6,7,8,9,10,11,12,13,2,3,5,4,7,9,8,7,11,13,12,2};
    int pages_found[RECOGNITION_COUNT];
    Mat page_images[NUM_PAGES];
    Mat blue_colour_sample;
    
    cout << "Loading Templates\n";
    // load all page images
    for(int i = 0; i < NUM_PAGES; i++){
        if(!load_image(get_page_path(i+1), &page_images[i])){
            return -1;
        }
    }
    cout << "Loading Blue Colour sample\n";
    // load blue colour sample image
    if(!(load_image(get_image_path(BLUE_COLOUR_SAMPLE), &blue_colour_sample))){
        return -1;
    }
    
    // Loop through each image to be recognised
    for(int i = 0; i < RECOGNITION_COUNT; i++){
        // load current image to process
        Mat image;
        if(!load_image(get_bookview_path(i+1), &image)){
            return -1;
        }
        
        cout << "Processing Image #" << i+1 << " ... ";
        
        // back projection
        Mat proc_image = back_project(image, blue_colour_sample, NUM_BINS_BLUE_BACK_PROJECT);
        
        // Convert to binary
        otsu_threshold(&proc_image);
        
        // closing operation
        binary_closing_operation(&proc_image);
        
        // get points
        vector<Point2f> blue_points = get_points(proc_image);
        
#ifdef DISPLAY_PROCESSING
        Mat display_processing;
        Mat temp_image1 = image.clone();
        temp_image1 = rescaleImage(temp_image1, 0.3);
        Mat temp_image2 = image.clone();
        draw_points(&temp_image2, blue_points);
        temp_image2 = rescaleImage(temp_image2, 0.3);
        display_processing = JoinImagesHorizontally(temp_image1, "Original Image", temp_image2, "Blue Points Found", 0, Scalar(0, 0, 255));
#endif
        
        // remove points that arent on page
        blue_points = remove_outlier_points(image, blue_points);
        
#ifdef DISPLAY_PROCESSING
        Mat temp_image3 = image.clone();
        draw_points(&temp_image3, blue_points);
        temp_image3 = rescaleImage(temp_image3, 0.3);
        display_processing = JoinImagesHorizontally(display_processing, "", temp_image3, "Removed Points Off Page", 0, Scalar(0, 0, 255));
        imshow("Processing Image #" + to_string(i+1), display_processing);
        cvWaitKey(0);
        cvDestroyAllWindows();
#endif
        
        // if the number of points is at least the number of expected points
        // proceed to template matching. Otherwise the image is recognised as having no page
        if((int)blue_points.size() >= NUM_POINTS_ON_PAGE){
            // get corners
            vector<Point2f> corners = get_corners(blue_points);
            
            // geometric transformation
            Mat transformed_image = geo_transform(image, corners);
            
            //pages_found[i] = sift_match(transformed_image, page_images, NUM_PAGES); // sift not working so well
            pages_found[i] = template_match(transformed_image, page_images, NUM_PAGES);
            pages_found[i] += 1; // increment by one because the page files start from one and not zero
            
#ifdef DISPLAY_RESULT
            Mat display_result = JoinImagesHorizontally(transformed_image, "Observed Image", page_images[pages_found[i] - 1], "Best Match", 0, Scalar(0, 0, 255));
            display_result = JoinImagesHorizontally(display_result, "", page_images[known_truth[i]-1], "Known Truth", 0, Scalar(0, 0, 255));
            imshow("Result For Image #" + to_string(i+1), display_result);
            cvWaitKey(0);
            cvDestroyAllWindows();
#endif
        }else{
            pages_found[i] = -1;
        }
        cout << "answer: " << pages_found[i] << "; known truth: " << known_truth[i] << endl;
    }
    
    // Measuring Performance
    
    int truePositives = 0, falsePositives = 0, trueNegatives = 0, falseNegatives = 0;
    for(int i = 0; i < RECOGNITION_COUNT; i++){
        if(pages_found[i] == known_truth[i])
            truePositives++;
        else if(pages_found[i] != -1 && known_truth[i] == -1){
            falsePositives++;
        }
        else if(pages_found[i] == -1 && known_truth[i] != -1){
            falseNegatives++;
        }
        else if(pages_found[i] == -1 && known_truth[i] == -1){
            trueNegatives++;
        }
    }
    
    float recall = truePositives / (truePositives + falseNegatives);
    float precision = truePositives / (truePositives + falsePositives);
    float accuracy = float(truePositives + trueNegatives) / (float)RECOGNITION_COUNT;
    float spec_denominator = (falsePositives + trueNegatives);
    float specificity;
    if(spec_denominator != 0){
        specificity = trueNegatives / spec_denominator;
    }else{
        specificity = 1.0;
    }
    float f1 = 2.0 * (precision * recall) / (precision + recall);
    
    cout << endl << "System Performance" << endl;
    cout << "True Positives: " << to_string(truePositives) << endl;
    cout << "False Positives: " << to_string(falsePositives)<< endl;
    cout << "True Negatives: " << to_string(trueNegatives)<< endl;
    cout << "False Negatives: " << to_string(falseNegatives)<< endl;
    cout << "System Recall: " << to_string(recall) << endl;
    cout << "System Precision: " << to_string(precision) << endl;
    cout << "System Accuracy: " << to_string(accuracy) << endl;
    cout << "System Specificity: " << to_string(specificity) << endl;
    cout << "System F1 Measure: " << to_string(f1) << endl;
    
    return 0;
}

vector<Point2f> remove_outlier_points(Mat image, vector<Point2f> points){
    vector<Point2f> result;
    for(int i = 0; i < (int)points.size(); i++){
        int top_left_x = points[i].x-(LENGTH_ROI_BLUE_POINTS/2);
        int top_left_y = points[i].y-(LENGTH_ROI_BLUE_POINTS/2);
        if(top_left_x >=0 && top_left_y >= 0){
            // get the roi around the point
            Rect region = Rect(top_left_x, top_left_y, LENGTH_ROI_BLUE_POINTS,LENGTH_ROI_BLUE_POINTS);
            Mat roi = image(region);
            cvtColor(roi, roi, CV_BGR2GRAY);
            otsu_threshold(&roi);
            // get the number of black pixels in B/W image
            int total_pixels = LENGTH_ROI_BLUE_POINTS * LENGTH_ROI_BLUE_POINTS;
            int black_pixels = total_pixels - countNonZero(roi);
            // if the number of black pixels is less than a third of the roi then add to result
            if(black_pixels < (total_pixels / RATIO_BLACK_PIXELS_ROI_BLUE_POINTS)){
                result.push_back(points[i]);
            }
        }
    }
    return result;
}

int sift_match(Mat image, Mat *templates, int num_templates){
    int best_match = -1;
    int highest_matches = 0;
    Mat gray_image1;
    cvtColor(image, gray_image1, CV_BGR2GRAY);
    for(int i = 0; i < num_templates; i++){
        cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
        //cv::Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
        //cv::Ptr<Feature2D> f2d = ORB::create();
        // you get the picture, i hope..
        
        Mat gray_image2;
        cvtColor(templates[i], gray_image2, CV_BGR2GRAY);
        
        //-- Step 1: Detect the keypoints:
        std::vector<KeyPoint> keypoints_1, keypoints_2;
        f2d->detect( gray_image1, keypoints_1 );
        f2d->detect( gray_image2, keypoints_2 );
        
        //-- Step 2: Calculate descriptors (feature vectors)
        Mat descriptors_1, descriptors_2;
        f2d->compute( gray_image1, keypoints_1, descriptors_1 );
        f2d->compute( gray_image2, keypoints_2, descriptors_2 );
        
        //-- Step 3: Matching descriptor vectors using BFMatcher :
        //BFMatcher matcher;
        FlannBasedMatcher matcher;
        std::vector< DMatch > matches;
        matcher.match( descriptors_1, descriptors_2, matches );
        
//        Mat display;
//        drawMatches(gray_image1, keypoints_1, gray_image2, keypoints_2, matches, display);
//        DisplayImage(display, "Matches", 50, 50);
//        cvWaitKey(0);
//        cvDestroyAllWindows();
        
        cout << "Number matches: " << (int)matches.size() << endl;
        
        if((int)matches.size() > highest_matches){
            best_match = i;
            highest_matches = (int)matches.size();
        }
    }
    return best_match;
}

int template_match(Mat image, Mat *templates, int num_templates){
    int best_match = -1;
    double highest = 0.0;
    for(int i = 0; i < num_templates; i++){
        Mat matching_space;
        matching_space.create(image.rows-templates[i].rows+1, image.cols-templates[i].cols+1, CV_32FC1);
        matchTemplate(image, templates[i], matching_space, CV_TM_CCORR_NORMED);
        double minVal; double maxVal; Point minLoc; Point maxLoc;
        minMaxLoc( matching_space, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
        if(maxVal > highest){
            best_match = i;
            highest = maxVal;
        }
    }
    return best_match;
}

Mat geo_transform(Mat image, vector<Point2f> corners){
    Point2f dest[4];
    dest[0] = Point2f(TEMPLATE_LEFT_CORNER_X, TEMPLATE_TOP_CORNER_Y); // top left
    dest[1] = Point2f(TEMPLATE_RIGHT_CORNER_X, TEMPLATE_TOP_CORNER_Y); // top right
    dest[2] = Point2f(TEMPLATE_LEFT_CORNER_X, TEMPLATE_BOTTOM_CORNER_Y); // bottom left
    dest[3] = Point2f(TEMPLATE_RIGHT_CORNER_X, TEMPLATE_BOTTOM_CORNER_Y); // bottom right
    
    Mat perspective_transform = getPerspectiveTransform(corners.data(), dest);
    Mat result = *new Mat();
    warpPerspective(image, result, perspective_transform, *new Size(TEMPLATE_WIDTH, TEMPLATE_HEIGHT));
    return result;
}

vector<Point2f> get_points(Mat binary_image){
    // get contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binary_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    vector<Point2f> points(contours.size());
    for(int i = 0; i < contours.size(); i++){
        points[i] = get_centre(contours[i]);
    }
    return points;
}

vector<Point2f> get_corners(vector<Point2f> points){
    int num_points = (int)points.size();
    assert(num_points >= 4);
    vector<Point2f> corners(4);
    Point2f top = points[0], left = points[0], right = points[0], bottom = points[0];
    for(int i = 1; i < num_points; i++){
        // top
        if(points[i].y < top.y){
            top = points[i];
        }
        // left
        if(points[i].x < left.x){
            left = points[i];
        }
        // right
        if(points[i].x > right.x){
            right = points[i];
        }
        // bottom
        if(points[i].y > bottom.y){
            bottom = points[i];
        }
    }
    if(left.y  < right.y){ // left higher than right
        corners[TOP_LEFT_CORNER] = left;
        corners[TOP_RIGHT_CORNER] = top;
        corners[BOTTOM_LEFT_CORNER] = bottom;
        corners[BOTTOM_RIGHT_CORNER] = right;
    }else{ // right higher than left
        corners[TOP_LEFT_CORNER] = top;
        corners[TOP_RIGHT_CORNER] = right;
        corners[BOTTOM_LEFT_CORNER] = left;
        corners[BOTTOM_RIGHT_CORNER] = bottom;
    }
    return corners;
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
