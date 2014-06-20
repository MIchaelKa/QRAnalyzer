//
//  CorrectFinderPattern.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 20/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef __QRAnalyzer__CorrectFinderPattern__
#define __QRAnalyzer__CorrectFinderPattern__

#include "DecodeEngineDefines.h"

class CorrectFinderPattern
{
public:
    CorrectFinderPattern();
    
    void initWithRotatedRect(cv::RotatedRect rect);
    void setCVMat(cv::Mat mat);
    void correctPoints();
    
    cv::Point2f getULPoint();
    cv::Point2f getURPoint();
    cv::Point2f getBLPoint();
    cv::Point2f getBRPoint();
    
    int getStep();   
    
private:
    
    void correctULPoint();
    void correctURPoint();
    void correctBLPoint();
    void correctBRPoint();
    
    cv::Point2f m_ULPoint;
    cv::Point2f m_URPoint;
    cv::Point2f m_BLPoint;
    cv::Point2f m_BRPoint;
    
    int m_step;
    
    cv::Mat m_frameThreshold;
};

#endif /* defined(__QRAnalyzer__CorrectFinderPattern__) */
