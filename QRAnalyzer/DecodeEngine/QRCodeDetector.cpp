//
//  QRCodeDetector.cpp
//  QRAnalyzer
//
//  Created by Michael Kalinin on 10/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#include "QRCodeDetector.h"
#include <stdio.h>

QRCodeDetector::QRCodeDetector()
{
    
}

bool QRCodeDetector::detectRedColor(cv::Mat frame)
{
    cv::Mat HSVFrame;
    
    cv::cvtColor(frame, HSVFrame, CV_BGR2HSV);
    
    cv::Point center(HSVFrame.cols / 2, HSVFrame.rows / 2);
    
    cv::Vec4b pixel = HSVFrame.at<cv::Vec4b>(center);
    
    uchar H = pixel[0];
    uchar S = pixel[1];
    uchar V = pixel[2];
    
    printf("H - %d, S - %d, V - %d \n", H, S, V);
    
    if (H > 160 && S > 220) return true;
    
    return false;
}
