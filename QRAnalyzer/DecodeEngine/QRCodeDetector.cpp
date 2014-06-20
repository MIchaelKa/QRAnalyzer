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
    
    correctFinderPatterns();
    
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
    
    if (!checkBasisPoints())
    {
        printf("NO finderPatterns POINTS\n");
        return false;
    }
    
    printf("%d - %d - %d - %d\n", m_ULFinderPattern.getStep(),
                                  m_URFinderPattern.getStep(),
                                  m_BLFinderPattern.getStep(),
                                  m_gridStep);
    
    //m_gridStep = m_ULFinderPattern.getStep();
    
    m_QRMatrixHeight = (m_BLBasisPoint.y - m_ULBasisPoint.y) / m_gridStep;
    m_QRMatrixWidth  = (m_URBasisPoint.x - m_ULBasisPoint.x) / m_gridStep;
    
    showGrid();
    showCorrectFinderPatterns();
    
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

void QRCodeDetector::correctFinderPatterns()
{
    // UL
    int centerX = m_finderPatterns[0].externalRect.center.x;
    int centerY = m_finderPatterns[0].externalRect.center.y;
    
    FinderPattern UL = m_finderPatterns[0];
    
    for (int k = 1; k < FINDER_PATTERNS; k++)
    {
        if (m_finderPatterns[k].externalRect.center.x < centerX &&
            m_finderPatterns[k].externalRect.center.y < centerY)
        {
            UL = m_finderPatterns[k];
            centerX = m_finderPatterns[k].externalRect.center.x;
            centerY = m_finderPatterns[k].externalRect.center.y;
        }
    }
    
    m_ULFinderPattern.setCVMat(m_frameThreshold);
    m_ULFinderPattern.initWithRotatedRect(UL.externalRect);
    m_ULFinderPattern.correctPoints();
    
    //UR
    centerX = m_finderPatterns[0].externalRect.center.x;
    
    FinderPattern UR = m_finderPatterns[0];
    
    for (int k = 1; k < FINDER_PATTERNS; k++)
    {
        if (m_finderPatterns[k].externalRect.center.x > centerX)
        {
            UR = m_finderPatterns[k];
            centerX = m_finderPatterns[k].externalRect.center.x;
        }
    }
    
    m_URFinderPattern.setCVMat(m_frameThreshold);
    m_URFinderPattern.initWithRotatedRect(UR.externalRect);
    m_URFinderPattern.correctPoints();
    
    // BL
    centerY = m_finderPatterns[0].externalRect.center.y;
    
    FinderPattern BL = m_finderPatterns[0];
    
    for (int k = 1; k < FINDER_PATTERNS; k++)
    {
        if (m_finderPatterns[k].externalRect.center.y > centerY)
        {
            BL = m_finderPatterns[k];
            centerY = m_finderPatterns[k].externalRect.center.y;
        }
    }
    
    m_BLFinderPattern.setCVMat(m_frameThreshold);
    m_BLFinderPattern.initWithRotatedRect(BL.externalRect);
    m_BLFinderPattern.correctPoints();
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
    int variance = m_finderPatterns[0].externalRect.size.width / 50;
    
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
    return m_ULFinderPattern.getULPoint();
}

cv::Point2f QRCodeDetector::getURBasisPoint()
{
    return m_URFinderPattern.getURPoint();
}

cv::Point2f QRCodeDetector::getBLBasisPoint()
{
    return m_BLFinderPattern.getBLPoint();
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

void QRCodeDetector::showCorrectFinderPatterns()
{
    showPoint(m_ULFinderPattern.getULPoint(), cv::Scalar(255, 0, 0));
    showPoint(m_ULFinderPattern.getURPoint(), cv::Scalar(255, 0, 0));
    showPoint(m_ULFinderPattern.getBLPoint(), cv::Scalar(255, 0, 0));
    showPoint(m_ULFinderPattern.getBRPoint(), cv::Scalar(255, 0, 0));
    
    showPoint(m_URFinderPattern.getULPoint(), cv::Scalar(0, 255, 0));
    showPoint(m_URFinderPattern.getURPoint(), cv::Scalar(0, 255, 0));
    showPoint(m_URFinderPattern.getBLPoint(), cv::Scalar(0, 255, 0));
    showPoint(m_URFinderPattern.getBRPoint(), cv::Scalar(0, 255, 0));
    
    showPoint(m_BLFinderPattern.getULPoint(), cv::Scalar(0, 0, 255));
    showPoint(m_BLFinderPattern.getURPoint(), cv::Scalar(0, 0, 255));
    showPoint(m_BLFinderPattern.getBLPoint(), cv::Scalar(0, 0, 255));
    showPoint(m_BLFinderPattern.getBRPoint(), cv::Scalar(0, 0, 255));
}

void QRCodeDetector::showRotatedRect(cv::RotatedRect rotatedRect, cv::Scalar color)
{
    cv::Point2f rect_points[4];
    rotatedRect.points(rect_points);
    for( int j = 0; j < 4; j++ ) {
        cv::line(m_frameOriginal, rect_points[j], rect_points[(j+1)%4], color);
    }
}

void QRCodeDetector::showPoint(cv::Point2f point, cv::Scalar color)
{
    int rectPointX = point.x;
    int rectPointY = point.y;
    
    m_frameOriginal.at<cv::Vec4b>(rectPointY, rectPointX)[0] = color[0];
    m_frameOriginal.at<cv::Vec4b>(rectPointY, rectPointX)[1] = color[1];
    m_frameOriginal.at<cv::Vec4b>(rectPointY, rectPointX)[2] = color[2];
}

void QRCodeDetector::showGrid()
{
    for (int i = 0; i < m_QRMatrixWidth + 1; i++)
    {
        for (int j = 0; j < m_QRMatrixHeight + 1; j++)
        {
            showPoint(cv::Point2f(m_ULBasisPoint.x + i * m_gridStep,
                                  m_ULBasisPoint.y + j * m_gridStep),
                      cv::Scalar(255, 255, 255));
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
