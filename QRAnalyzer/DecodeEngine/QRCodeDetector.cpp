//
//  QRCodeDetector.cpp
//  QRAnalyzer
//
//  Created by Michael Kalinin on 10/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#include "QRCodeDetector.h"

QRCodeDetector::QRCodeDetector()
{
    
}

bool QRCodeDetector::detectRedColor(cv::Mat frame)
{
    cv::Mat HSVFrame;
    cv::cvtColor(frame, HSVFrame, CV_BGR2HSV);
    
    
    return false;
}