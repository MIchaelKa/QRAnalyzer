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

bool QRCodeDetector::detectQRCode(cv::Mat& frame)
{
    cv::Mat frameGrayScale;
    cv::Mat frameThreshold;
    
    m_frameOriginal = frame;
    
    cv::cvtColor(frame, frameGrayScale, CV_BGR2GRAY);
    cv::threshold(frameGrayScale, frameThreshold, 150, WHITE_PIXEL, CV_THRESH_BINARY);
    
    return identifyFinderPatterns(frameThreshold);
}

bool QRCodeDetector::identifyFinderPatterns(cv::Mat& mat)
{
    //Find the contours of finder patterns 
    vector<vector<cv::Point>> contours;
    vector<vector<cv::Point>> targetContours;

    vector<cv::RotatedRect>   finderPatternRects;
    
    vector<FinderPattern> finderPatterns;
    
    cv::Mat contourOutput = mat.clone();
    cv::findContours(contourOutput, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
  
    for(int i = 0; i < contours.size(); i++)
    {
        cv::RotatedRect rotatedRect = cv::minAreaRect(cv::Mat(contours[i]));
        int contourArea = cv::contourArea(contours[i]);
        int rotateRectArea = rotatedRect.size.width * rotatedRect.size.height;
        int variance = contourArea / 5;

        if (abs(contourArea - rotateRectArea) < variance &&
            contourArea > MIN_COUNTUR_AREA)
        {
            targetContours.push_back(contours[i]);
        }
    }
    
    if (targetContours.size() < FINDER_PATTERNS * FINDER_SECTIONS)
    {
        return false;
    }
    
    m_finderPatternRects.clear();
    
    for (int i = 0; i < targetContours.size(); i++)
    {
        cv::RotatedRect firstRect = cv::minAreaRect(cv::Mat(targetContours[i]));
        for (int j = 0; j < targetContours.size(); j++)
        {
            if (i != j)
            {
                cv::RotatedRect secondRect = cv::minAreaRect(cv::Mat(targetContours[j]));
                if (rectIsContainInnerRect(firstRect, secondRect))
                {
                    addNewFinderPatternRect(firstRect);
                    addNewFinderPatternRect(secondRect);
                }
            }
        }
    }
    
    printf("%d\n", (int)m_finderPatternRects.size());
    
    if (m_finderPatternRects.size() < FINDER_PATTERNS * FINDER_SECTIONS)
    {
        return false;
    } 
    
    
    //Draw the contours
    cv::Scalar colors[3];
    colors[0] = cv::Scalar(255, 0, 0);
    colors[1] = cv::Scalar(0, 255, 0);
    colors[2] = cv::Scalar(0, 0, 255);
    
//    for (int idx = 0; idx < targetContours.size(); idx++) {
//        cv::drawContours(m_frameOriginal, targetContours, idx, colors[idx % 3]);
//    }
    
    for (int idx = 0; idx < m_finderPatternRects.size(); idx++) {
        cv::Point2f rect_points[4];
        m_finderPatternRects[idx].points(rect_points);
        for( int j = 0; j < 4; j++ ) {
            cv::line(m_frameOriginal, rect_points[j], rect_points[(j+1)%4], colors[idx % 3]);
        }
    }    
    
    return true;
}

void QRCodeDetector::addNewFinderPatternRect(cv::RotatedRect& rect)
{
    for (int i = 0; i < m_finderPatternRects.size(); i++) {
        if (rectsIsEqual(rect, m_finderPatternRects[i]))
            return;
    }
    m_finderPatternRects.push_back(rect);
}

bool QRCodeDetector::rectsIsEqual(cv::RotatedRect& firstRect,
                                  cv::RotatedRect& secondRect)
{
    if ((int)firstRect.center.x != (int)secondRect.center.x ||
        (int)firstRect.center.y != (int)secondRect.center.y)
    {
        return false;
    }
    if ((int)firstRect.size.width  != (int)secondRect.size.width ||
        (int)firstRect.size.height != (int)secondRect.size.height)
    {
        return false;
    }
    return true;
}

bool QRCodeDetector::rectIsContainInnerRect(cv::RotatedRect& externalRect,
                                            cv::RotatedRect& innerRect)
{
    int variance = 2;
    if (abs(externalRect.center.x - innerRect.center.x) < variance &&
        abs(externalRect.center.y - innerRect.center.y) < variance)
    {
        if (externalRect.size.width  >= innerRect.size.width &&
            externalRect.size.height >= innerRect.size.height)
        {
            return true;
        }
    }
    return false;
}
