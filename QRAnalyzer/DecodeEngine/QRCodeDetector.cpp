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
    m_gridStep = 0;
}

bool QRCodeDetector::detectQRCode(cv::Mat& frame)
{
    cv::Mat frameGrayScale;
    cv::Mat frameThreshold;
    
    m_frameOriginal = frame;
    
    cv::cvtColor(frame, frameGrayScale, CV_BGR2GRAY);
    cv::threshold(frameGrayScale, frameThreshold, 150, WHITE_PIXEL, CV_THRESH_BINARY);
    
    m_frameThreshold = frameThreshold;
    
    return identifyFinderPatterns(m_frameThreshold);
}

bool QRCodeDetector::identifyFinderPatterns(cv::Mat& mat)
{
    //Find the contours of finder patterns 
    vector<vector<cv::Point>> contours;
    vector<vector<cv::Point>> targetContours;
    
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
        printf("NO targetContours\n");
        return false;
    }
    
    m_finderPatternRects.clear();
    m_finderPatterns.clear();
    
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
    
    if (m_finderPatternRects.size() < FINDER_PATTERNS * FINDER_SECTIONS)
    {
        printf("NO finderPatternRects\n");
        return false;
    }
   
    for (int k = 0; k < FINDER_PATTERNS; k++)
    {
        FinderPattern finderPattern;
        finderPattern.externalRect = m_finderPatternRects.back();
        m_finderPatternRects.pop_back();
        
        for (int i = 0; i < m_finderPatternRects.size(); i++)
        {
            cv::RotatedRect rect = m_finderPatternRects[i];
            if (rectsAtTheSameCenter(finderPattern.externalRect, rect))
            {
                addToFinderPattern(finderPattern, rect);
                m_finderPatternRects.erase(m_finderPatternRects.begin() + i);
                i--;
            }
        }
        m_finderPatterns.push_back(finderPattern);
    }
    
    if (!checkRatio())
    {
        printf("NO finderPatterns RATIO\n");
        return false;
    }
    
    if (!checkSize())
    {
        printf("NO finderPatterns SIZE\n");
        return false;
    }
    
    //showFinderPatterns();
    
    m_ULBasisPoint = getULBasisPoint();
    m_URBasisPoint = getURBasisPoint();
    m_BLBasisPoint = getBLBasisPoint();
    
    correctULBasisPoint();
    correctURBasisPoint();
    
    showPoint(m_URBasisPoint);
    
    m_QRMatrixHeight = (m_BLBasisPoint.y - m_ULBasisPoint.y) / m_gridStep;
    m_QRMatrixWidth  = (m_URBasisPoint.x - m_ULBasisPoint.x) / m_gridStep;
    
    if (!checkBasisPoints())
    {
        printf("NO finderPatterns POINTS\n");
        return false;
    }
    
    showGrid();
    
    fillQRMatrix();
    printQRMatrix();
    
    printf("%d, %d\n", m_QRMatrixHeight, m_QRMatrixWidth);
    return true;
}

bool QRCodeDetector::addToFinderPattern(FinderPattern& finderPattern,
                                        cv::RotatedRect& rect)
{
    int externalRectSize = finderPattern.externalRect.size.width;
    int middleRectSize   = finderPattern.middleRect.size.width;
    
    int addedRectSize    = rect.size.width;
    
    if (externalRectSize < addedRectSize)
    {
        finderPattern.innerRect    = finderPattern.middleRect;
        finderPattern.middleRect   = finderPattern.externalRect;
        finderPattern.externalRect = rect;
        return true;
    }
    if (middleRectSize < addedRectSize)
    {
        finderPattern.innerRect    = finderPattern.middleRect;
        finderPattern.middleRect = rect;
        return true;
    }
    finderPattern.innerRect = rect;
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

bool QRCodeDetector::checkSize()
{
    int variance = 3;
    
    int firstRectWidth = m_finderPatterns[0].externalRect.size.width;
    int secondRectWidth = m_finderPatterns[1].externalRect.size.width;
    int thirdRectWidth = m_finderPatterns[2].externalRect.size.width;
    
    if (abs(firstRectWidth  - secondRectWidth) > variance ||
        abs(firstRectWidth  - thirdRectWidth)  > variance ||
        abs(secondRectWidth - thirdRectWidth)  > variance)
    {
        return false;
    }
    
    int firstRectHeight  = m_finderPatterns[0].externalRect.size.height;
    int secondRectHeight = m_finderPatterns[1].externalRect.size.height;
    int thirdRectHeight  = m_finderPatterns[2].externalRect.size.height;
    
    if (abs(firstRectHeight  - secondRectHeight) > variance ||
        abs(firstRectHeight  - secondRectHeight) > variance ||
        abs(secondRectHeight - thirdRectHeight)  > variance)
    {
        return false;
    }
    
    return true;
    
}

bool QRCodeDetector::checkRatio()
{
    int variance = m_finderPatterns[0].externalRect.size.width / 10;
    
    for (int k = 0; k < FINDER_PATTERNS; k++)
    {
        int externalRectSize = m_finderPatterns[k].externalRect.size.width;
        int middleRectSize   = m_finderPatterns[k].middleRect.size.width;
        int innerRectSize    = m_finderPatterns[k].innerRect.size.width;
        
        int externalRatio = externalRectSize / EXTERNAL_RECT_RATIO;
        int middleRatio   = middleRectSize   / MIDDLE_RECT_RATIO;
        int innerRatio    = innerRectSize    / INNER_RECT_RATIO;
        
        if (abs(externalRatio - middleRatio) > variance ||
            abs(externalRatio - innerRatio)  > variance ||
            abs(middleRatio   - innerRatio)  > variance)
        {
            return false;
        }
        
        if (m_gridStep != 0 && abs(m_gridStep - externalRatio) > variance)
        {
            return false;
        }
        
        m_gridStep = externalRatio;
    }
    return true;
}

bool QRCodeDetector::checkBasisPoints()
{
    int variance = 1;
    
    if (abs(m_ULBasisPoint.y - m_URBasisPoint.y) > variance ||
        abs(m_ULBasisPoint.x - m_BLBasisPoint.x) > variance)
    {
        return false;
    }
    
    return true;
}

void QRCodeDetector::fillQRMatrix()
{
    m_QRMatrix = new int* [m_QRMatrixHeight];
    
    for (int i = 0; i < m_QRMatrixHeight; i++)
    {
        m_QRMatrix[i] = new int [m_QRMatrixWidth];
    }
    
    for (int i = 0; i < m_QRMatrixHeight; i++)
    {
        for (int j = 0; j < m_QRMatrixWidth; j++)
        {
            if (m_frameThreshold.at<uchar>(m_ULBasisPoint.y + i * m_gridStep + m_gridStep/2,
                                           m_ULBasisPoint.x + j * m_gridStep + m_gridStep/2)
                == BLACK_PIXEL)
            {
                m_QRMatrix[i][j] = 1;
            }
            else
            {
                m_QRMatrix[i][j] = 0;
            }
        }
    }
    
}

cv::Point2f QRCodeDetector::getULBasisPoint()
{
    cv::Point2f ULPoint;
    for (int k = 0; k < FINDER_PATTERNS; k++)
    {
        cv::Point2f rect_points[4];
        m_finderPatterns[k].externalRect.points(rect_points);
        for (int i = 0; i < 4; i++)
        {
            if (ULPoint.x == 0 && ULPoint.y == 0)
            {
                ULPoint = rect_points[i];
            }
            if ((rect_points[i].x + rect_points[i].y) < (ULPoint.x + ULPoint.y))
            {
                ULPoint = rect_points[i];
            }
        }
    }
    showPoint(ULPoint);
    return ULPoint;
}

void QRCodeDetector::correctULBasisPoint()
{
    int x = m_ULBasisPoint.x;
    int y = m_ULBasisPoint.y;
    
    int searchRange = 5;
    
    for (int i = 1; i < searchRange; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            if (m_frameThreshold.at<uchar>(y + i - j, x + j) == BLACK_PIXEL)
            {
                m_ULBasisPoint.x = x + j;
                m_ULBasisPoint.y = y + i - j;
                return;
            }
        }
    }
}

cv::Point2f QRCodeDetector::getURBasisPoint()
{
    cv::Point2f URPoint;
    for (int k = 0; k < FINDER_PATTERNS; k++)
    {
        cv::Point2f rect_points[4];
        m_finderPatterns[k].externalRect.points(rect_points);
        for (int i = 0; i < 4; i++)
        {
            if (URPoint.x == 0 && URPoint.y == 0)
            {
                URPoint = rect_points[i];
            }
            if ((rect_points[i].x - rect_points[i].y) > (URPoint.x - URPoint.y))
            {
                URPoint = rect_points[i];
            }
        }
    }
    showPoint(URPoint);
    return URPoint;
}

void QRCodeDetector::correctURBasisPoint()
{
    int x = m_URBasisPoint.x;
    int y = m_URBasisPoint.y;
    
    int searchRange = 5;
    
    for (int i = 1; i < searchRange; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            if (m_frameThreshold.at<uchar>(y + i - j, x -j) == BLACK_PIXEL)
            {
                m_URBasisPoint.x = x - j;
                m_URBasisPoint.y = y + i - j;
                return;
            }
        }
    }
}

cv::Point2f QRCodeDetector::getBLBasisPoint()
{
    cv::Point2f BLPoint;
    for (int k = 0; k < FINDER_PATTERNS; k++)
    {
        cv::Point2f rect_points[4];
        m_finderPatterns[k].externalRect.points(rect_points);
        for (int i = 0; i < 4; i++)
        {
            if (BLPoint.x == 0 && BLPoint.y == 0)
            {
                BLPoint = rect_points[i];
            }
            if ((rect_points[i].y - rect_points[i].x) > (BLPoint.y - BLPoint.x))
            {
                BLPoint = rect_points[i];
            }
        }
    }
    showPoint(BLPoint);
    return BLPoint;
}

bool QRCodeDetector::rectsAtTheSameCenter(cv::RotatedRect& firstRect,
                                          cv::RotatedRect& secondRect)
{
    int variance = 4;
    if (abs(firstRect.center.x - secondRect.center.x) < variance &&
        abs(firstRect.center.y - secondRect.center.y) < variance)
    {
        return true;
    }
    return false;
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

void QRCodeDetector::showFinderPatterns()
{
    cv::Scalar colors[3];
    colors[0] = cv::Scalar(255, 0, 0);
    colors[1] = cv::Scalar(0, 255, 0);
    colors[2] = cv::Scalar(0, 0, 255);

    for (int k = 0; k < FINDER_PATTERNS; k++)
    {
        showRotatedRect(m_finderPatterns[k].externalRect, colors[k]);
        showRotatedRect(m_finderPatterns[k].middleRect,   colors[k]);
        showRotatedRect(m_finderPatterns[k].innerRect,    colors[k]);
    }
}

void QRCodeDetector::showRotatedRect(cv::RotatedRect rotatedRect, cv::Scalar color)
{
    cv::Point2f rect_points[4];
    rotatedRect.points(rect_points);
    for( int j = 0; j < 4; j++ ) {
        cv::line(m_frameOriginal, rect_points[j], rect_points[(j+1)%4], color);
    }
    
    //showPoint(rect_points[0]);
}

void QRCodeDetector::showPoint(cv::Point2f point)
{
    
    int rectPointX = point.x;
    int rectPointY = point.y;
    
    m_frameOriginal.at<cv::Vec4b>(rectPointY, rectPointX)[0] = 255;
    m_frameOriginal.at<cv::Vec4b>(rectPointY, rectPointX)[1] = 255;
    m_frameOriginal.at<cv::Vec4b>(rectPointY, rectPointX)[2] = 255;
}

void QRCodeDetector::showGrid()
{
    for (int i = 0; i < m_QRMatrixWidth + 1; i++)
    {
        for (int j = 0; j < m_QRMatrixHeight + 1; j++)
        {
            showPoint(cv::Point2f(m_ULBasisPoint.x + i * m_gridStep,
                                  m_ULBasisPoint.y + j * m_gridStep));
        }
    }
}

void QRCodeDetector::printQRMatrix()
{
    for (int i = 0; i < m_QRMatrixHeight; i++)
    {
        for (int j = 0; j < m_QRMatrixWidth; j++)
        {
            printf("%d ", m_QRMatrix[i][j]);
        }
        printf("\n");
    }
}
