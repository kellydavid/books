//
//  histogram.cpp
//  books
//
// The code for histogram.cpp is largely based on the code from the book
// A practical guide to Computer Vision with OpenCV by Kenneth Dawson-Howe
//
//  Created by David Kelly on 23/11/2015.
//  Copyright © 2015 David Kelly. All rights reserved.
//

#include "histogram.hpp"

// Histogram class

Histogram::Histogram( Mat image, int number_of_bins )
{
    mImage = image;
    mNumberChannels = mImage.channels();
    mChannelNumbers = new int[mNumberChannels];
    mNumberBins = new int[mNumberChannels];
    mChannelRange[0] = 0.0;
    mChannelRange[1] = 255.0;
    for (int count=0; count<mNumberChannels; count++)
    {
        mChannelNumbers[count] = count;
        mNumberBins[count] = number_of_bins;
    }
    //ComputeHistogram();
}

void Histogram::Draw1DHistogram( MatND histograms[], int number_of_histograms, Mat& display_image )
{
    int number_of_bins = histograms[0].size[0];
    double max_value=0, min_value=0;
    double channel_max_value=0, channel_min_value=0;
    for (int channel=0; (channel < number_of_histograms); channel++)
    {
        minMaxLoc(histograms[channel], &channel_min_value, &channel_max_value, 0, 0);
        max_value = ((max_value > channel_max_value) && (channel > 0)) ? max_value : channel_max_value;
        min_value = ((min_value < channel_min_value) && (channel > 0)) ? min_value : channel_min_value;
    }
    float scaling_factor = ((float)256.0)/((float)number_of_bins);
    
    Mat histogram_image((int)(((float)number_of_bins)*scaling_factor)+1,(int)(((float)number_of_bins)*scaling_factor)+1,CV_8UC3,Scalar(255,255,255));
    display_image = histogram_image;
    line(histogram_image,Point(0,0),Point(0,histogram_image.rows-1),Scalar(0,0,0));
    line(histogram_image,Point(histogram_image.cols-1,histogram_image.rows-1),Point(0,histogram_image.rows-1),Scalar(0,0,0));
    int highest_point = static_cast<int>(0.9*((float)number_of_bins)*scaling_factor);
    for (int channel=0; (channel < number_of_histograms); channel++)
    {
        int last_height;
        for( int h = 0; h < number_of_bins; h++ )
        {
            float value = histograms[channel].at<float>(h);
            int height = static_cast<int>(value*highest_point/max_value);
            int where = (int)(((float)h)*scaling_factor);
            if (h > 0)
                line(histogram_image,Point((int)(((float)(h-1))*scaling_factor)+1,(int)(((float)number_of_bins)*scaling_factor)-last_height),
                     Point((int)(((float)h)*scaling_factor)+1,(int)(((float)number_of_bins)*scaling_factor)-height),
                     Scalar(channel==0?255:0,channel==1?255:0,channel==2?255:0));
            last_height = height;
        }
    }
}

// OneDHistogram Class

OneDHistogram::OneDHistogram( Mat image, int number_of_bins ) :
Histogram( image, number_of_bins ){
    ComputeHistogram( );
}
void OneDHistogram::ComputeHistogram(){
    vector<Mat> image_planes(mNumberChannels);
    split(mImage, image_planes);
    for (int channel=0; (channel < mNumberChannels); channel++)
    {
        const float* channel_ranges = mChannelRange;
        int *mch = {0};
        calcHist(&(image_planes[channel]), 1, mChannelNumbers, Mat(), mHistogram[channel], 1 , mNumberBins, &channel_ranges);
    }
}

void OneDHistogram::SmoothHistogram( ){
    for (int channel=0; (channel < mNumberChannels); channel++)
    {
        MatND temp_histogram = mHistogram[channel].clone();
        for(int i = 1; i < mHistogram[channel].rows - 1; ++i)
        {
            mHistogram[channel].at<float>(i) = (temp_histogram.at<float>(i-1) + temp_histogram.at<float>(i) + temp_histogram.at<float>(i+1)) / 3;
        }
    }
}

MatND OneDHistogram::getHistogram(int index){
    return mHistogram[index];
}

void OneDHistogram::NormaliseHistogram(){
    for (int channel=0; (channel < mNumberChannels); channel++)
    {
        normalize(mHistogram[channel],mHistogram[channel],1.0);
    }
}

Mat OneDHistogram::BackProject( Mat& image ){
    Mat&& result = image.clone();
    if (mNumberChannels == 1)
    {
        const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
        for (int channel=0; (channel < mNumberChannels); channel++)
        {
            calcBackProject(&image,1,mChannelNumbers,*mHistogram,result,channel_ranges,255.0);
        }
    }
    else
    {
    }
    return result;
}

void OneDHistogram::Draw( Mat& display_image ){
    Draw1DHistogram( mHistogram, mNumberChannels, display_image );
}

// ColourHistogram Class

ColourHistogram::ColourHistogram( Mat image, int number_of_bins ) :
Histogram( image, number_of_bins ){
    ComputeHistogram();
}

void ColourHistogram::ComputeHistogram(){
    const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
    calcHist(&mImage, 1, mChannelNumbers, Mat(), mHistogram, mNumberChannels, mNumberBins, channel_ranges);
}

void ColourHistogram::NormaliseHistogram(){
    normalize(mHistogram,mHistogram,1.0);
}

Mat ColourHistogram::BackProject( Mat& image ){
    Mat&& result = image.clone();
    const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
    calcBackProject(&image,1,mChannelNumbers,mHistogram,result,channel_ranges,255.0);
    return result;
}

MatND ColourHistogram::getHistogram(){
    return mHistogram;
}

// HueHistogram Class

HueHistogram::HueHistogram( Mat image, int number_of_bins, int min_saturation, int min_value, int max_value) :
Histogram( image, number_of_bins ){
    mMinimumSaturation = min_saturation;
    mMinimumValue = min_value;
    mMaximumValue = max_value;
    mChannelRange[1] = 180.0;
    ComputeHistogram();
}

void HueHistogram::ComputeHistogram(){
    Mat hsv_image, hue_image, mask_image;
    cvtColor(mImage, hsv_image, CV_BGR2HSV);
    inRange( hsv_image, Scalar( 0, mMinimumSaturation, mMinimumValue ), Scalar( 180, 256, mMaximumValue ), mask_image );
    int channels[]={0,0};
    hue_image.create( mImage.size(), mImage.depth());
    mixChannels( &hsv_image, 1, &hue_image, 1, channels, 1 );
    const float* channel_ranges = mChannelRange;
    calcHist( &hue_image,1,0,mask_image,mHistogram,1,mNumberBins,&channel_ranges);
}

void HueHistogram::NormaliseHistogram(){
    normalize(mHistogram,mHistogram,0,255,CV_MINMAX);
}

Mat HueHistogram::BackProject( Mat& image ){
    Mat&& result = image.clone();
    const float* channel_ranges = mChannelRange;
    calcBackProject(&image,1,mChannelNumbers,mHistogram,result,&channel_ranges,255.0);
    return result;
}

MatND HueHistogram::getHistogram(){
    return mHistogram;
}

void HueHistogram::Draw( Mat& display_image ){
    Draw1DHistogram( &mHistogram, 1, display_image );
}

// k means
Mat kmeans_clustering( Mat& image, int k, int iterations )
{
    CV_Assert( image.type() == CV_8UC3 );
    // Populate an n*3 array of float for each of the n pixels in the image
    Mat samples(image.rows*image.cols, image.channels(), CV_32F);
    float* sample = samples.ptr<float>(0);
    for(int row=0; row<image.rows; row++)
        for(int col=0; col<image.cols; col++)
            for (int channel=0; channel < image.channels(); channel++)
                samples.at<float>(row*image.cols+col,channel) =
                (uchar) image.at<Vec3b>(row,col)[channel];
    // Apply k-means clustering to cluster all the samples so that each sample
    // is given a label and each label corresponds to a cluster with a particular
    // centre.
    Mat labels;
    Mat centres;
    kmeans(samples, k, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 1, 0.0001),
           iterations, KMEANS_PP_CENTERS, centres );
    // Put the relevant cluster centre values into a result image
    Mat&& result_image = Mat( image.size(), image.type() );
    for(int row=0; row<image.rows; row++)
        for(int col=0; col<image.cols; col++)
            for (int channel=0; channel < image.channels(); channel++)
                result_image.at<Vec3b>(row,col)[channel] = (uchar) centres.at<float>(*(labels.ptr<int>(row*image.cols+col)), channel);
    return result_image;
}