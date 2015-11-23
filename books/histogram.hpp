//
//  histogram.hpp
//  books
//
// The code for histogram.hpp is largely based on the code from the book
// A practical guide to Computer Vision with OpenCV by Kenneth Dawson-Howe
//
//  Created by David Kelly on 23/11/2015.
//  Copyright © 2015 David Kelly. All rights reserved.
//

#ifndef histogram_hpp
#define histogram_hpp

#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

// For Hue Saturation
#define DEFAULT_MIN_SATURATION 25
#define DEFAULT_MIN_VALUE 25
#define DEFAULT_MAX_VALUE 230

using namespace std;
using namespace cv;

class Histogram
{
protected:
    Mat mImage;
    int mNumberChannels;
    int* mChannelNumbers;
    int* mNumberBins;
    float mChannelRange[2];
public:
    Histogram( Mat image, int number_of_bins );
    virtual void ComputeHistogram()=0;
    virtual void NormaliseHistogram()=0;
    static void Draw1DHistogram( MatND histograms[], int number_of_histograms, Mat& display_image );
};

class OneDHistogram : public Histogram
{
private:
    MatND mHistogram[3];
public:
    OneDHistogram( Mat image, int number_of_bins );
    void ComputeHistogram();
    void SmoothHistogram( );
    MatND getHistogram(int index);
    void NormaliseHistogram();
    Mat BackProject( Mat& image );
    void Draw( Mat& display_image );
};

class ColourHistogram : public Histogram
{
private:
    MatND mHistogram;
public:
    ColourHistogram( Mat image, int number_of_bins );
    void ComputeHistogram();
    void NormaliseHistogram();
    Mat BackProject( Mat& image );
    MatND getHistogram();
};

class HueHistogram : public Histogram
{
private:
    MatND mHistogram;
    int mMinimumSaturation, mMinimumValue, mMaximumValue;
public:
    HueHistogram( Mat image, int number_of_bins, int min_saturation=DEFAULT_MIN_SATURATION, int min_value=DEFAULT_MIN_VALUE, int max_value=DEFAULT_MAX_VALUE );
    void ComputeHistogram();
    void NormaliseHistogram();
    Mat BackProject( Mat& image );
    MatND getHistogram();
    void Draw( Mat& display_image );
};

Mat kmeans_clustering( Mat& image, int k, int iterations );

#endif /* histogram_hpp */
