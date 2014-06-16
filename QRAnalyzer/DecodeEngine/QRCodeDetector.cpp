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
    
    return findQRCodePosition(frameThreshold);
}

bool QRCodeDetector::findQRCodePosition(cv::Mat& mat)
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


bool QRCodeDetector::identifyFinderPatterns(cv::Mat& mat)
{
    int pixelCount[NUMBER_OF_SECTION] = { 0 };
    int currentSection = 0;
    PixelValue currentValue = BLACK_PIXEL;
    int i = 0;
    int matchedLines = 0;
    cv::Point scanLine [2];
    
    for (int k = 0; k < mat.rows; k++)
    {
        while (mat.at<uchar>(k, i) != BLACK_PIXEL && i < mat.cols) i++;
        scanLine[0] = cv::Point(k, i);
        for ( ; i < mat.cols; i++)
        {
            uchar value = mat.at<uchar>(k, i);
            
            if (value != currentValue)
            {
                if (currentSection == 1) scanLine[1] = cv::Point(k, i);
                if (currentSection < NUMBER_OF_SECTION - 1)
                {
                    currentValue = invertPixelValue(currentValue);
                    currentSection++;
                }
                else
                {
                    if (checkRatio(pixelCount))
                    {
                        scanLine[1] = cv::Point(k, i);
                        for (int j = scanLine[0].y; j < scanLine[1].y; j++)
                        {
                            //mat.at<uchar>(scanLine[0].x, j) = 150;
                            
                        }
                        matchedLines++;
                        printf("%d) - %d %d %d %d %d\n",
                               matchedLines, pixelCount[0], pixelCount[1], pixelCount[2], pixelCount[3], pixelCount[4]);
                        
                        while (mat.at<uchar>(k, i) != BLACK_PIXEL && i < mat.cols) i++;
                        currentSection = 0;
                        currentValue = BLACK_PIXEL;
                        pixelCount[0] = 0;
                        pixelCount[1] = 0;
                        pixelCount[2] = 0;
                        pixelCount[3] = 0;
                        pixelCount[4] = 0;
                    }
                    else
                    {
                        scanLine[0] = scanLine[1];
                        pixelCount[0] = pixelCount[2];
                        pixelCount[1] = pixelCount[3];
                        pixelCount[2] = pixelCount[4];
                        pixelCount[3] = 0;
                        pixelCount[4] = 0;
                        currentSection = 3;
                    }
                }
            }
            pixelCount[currentSection]++;
        }
        i = 0;
        currentSection = 0;        
        currentValue = BLACK_PIXEL;
        pixelCount[0] = 0;
        pixelCount[1] = 0;
        pixelCount[2] = 0;
        pixelCount[3] = 0;
        pixelCount[4] = 0;
    }
    
    if (matchedLines >= 20) return true;
    
    return false;
}

bool QRCodeDetector::checkRatio(int pixelCount[])
{
    int totalCount = 0;
    for (int i = 0; i < NUMBER_OF_SECTION; i++)
    {
        if (pixelCount[i] < 2) return false;
        totalCount += pixelCount[i];
    }
    
    if (totalCount < 7) return false;
    
    int sectionSize = totalCount / 7;
    int variance = sectionSize / 2;
    
    bool retVal = ((abs(sectionSize     - (pixelCount[0])) < variance) &&
                   (abs(sectionSize     - (pixelCount[1])) < variance) &&
                   (abs(3 * sectionSize - (pixelCount[2])) < variance) &&
                   (abs(sectionSize     - (pixelCount[3])) < variance) &&
                   (abs(sectionSize     - (pixelCount[4])) < variance));
    
    return retVal;
}

void QRCodeDetector::clearPixelCount()
{

}

PixelValue QRCodeDetector::invertPixelValue(PixelValue value)
{
    if (value == WHITE_PIXEL) return BLACK_PIXEL;
    else return WHITE_PIXEL;
}
