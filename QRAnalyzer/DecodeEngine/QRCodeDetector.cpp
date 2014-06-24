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
    m_QRMatrixDecoder = new QRMatrixDecoder();
    m_gridWidthStep  = 0;
    m_gridHeightStep = 0;
}

bool QRCodeDetector::detectQRCode(cv::Mat& frame)
{
    cv::Mat frameGrayScale;
    cv::Mat frameThreshold;
    
    m_frameOriginal = frame;
    
    cv::cvtColor(frame, frameGrayScale, CV_BGR2GRAY);
    cv::threshold(frameGrayScale, frameThreshold, 150, WHITE_PIXEL, CV_THRESH_BINARY);
    
    m_frameThreshold = frameThreshold;
    
    if (identifyFinderPatterns(m_frameThreshold))
    {
        frame = m_frameAligned;
        m_QRMatrixDecoder->setQRMatrix(m_QRMatrix, m_QRMatrixSize);
        m_QRMatrixDecoder->decodeQRMatrix();
        return true;
    }
    return false;
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
    
    if (!checkRatio()) // for corrected patterns
    {
        printf("NO finderPatterns RATIO\n");
        return false;
    }
    
    m_ULBasisPoint = getULBasisPoint();
    m_URBasisPoint = getURBasisPoint();
    m_BLBasisPoint = getBLBasisPoint();
    
    if (!checkBasisPoints())
    {
        printf("NO finderPatterns POINTS\n");
        return false;
    }
    
    calculateQRMatrixSize((m_BLBasisPoint.y - m_ULBasisPoint.y) / m_gridWidthStep);
    
    if (m_QRMatrixSize == -1)
    {
        printf("INCORRECT MATRIX SIZE\n");
        return false;
    }
    
    alignQRCodeRatio();
    deskewQRCanvas();
    
    fillQRMatrix();
    printQRMatrix();
    
    printf("%d\n", m_QRMatrixSize);
    return true;
}

#pragma mark QRCodeDetector - Finder pattern adding

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

#pragma mark QRCodeDetector - Checkers and correction methods

void QRCodeDetector::deskewQRCanvas()
{
    cv::Point2f srcTri[3];
    cv::Point2f dstTri[3];
    
    cv::Mat rot_mat( 2, 3, CV_32FC1 );
    cv::Mat warp_mat( 2, 3, CV_32FC1 );
    
    // Set the dst image the same type and size as src
    m_frameAligned = cv::Mat::zeros(m_QRCanvasHeight,
                                    m_QRCanvasWidth,
                                    m_frameThreshold.type());
    
    // Set your 3 points to calculate the  Affine Transform
    srcTri[0] = m_ULBasisPoint;
    srcTri[1] = m_URBasisPoint;
    srcTri[2] = m_BLBasisPoint;
    
    dstTri[0] = cv::Point2f(0, 0);
    dstTri[1] = cv::Point2f(m_QRCanvasWidth, 0);
    dstTri[2] = cv::Point2f(0, m_QRCanvasHeight);
    
    // Get the Affine Transform
    warp_mat = cv::getAffineTransform(srcTri, dstTri);
    
    // Apply the Affine Transform just found to the src image
    cv::warpAffine(m_frameThreshold,
                   m_frameAligned,
                   warp_mat,
                   m_frameAligned.size());
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

bool QRCodeDetector::checkRatio()
{
    int variance = 5;
    
    int ULWidth  = m_ULFinderPattern.getWidth();
    int ULHeight = m_ULFinderPattern.getHeight();
    
    int URWidth  = m_URFinderPattern.getWidth();
    int URHeight = m_URFinderPattern.getHeight();
    
    int BLWidth  = m_BLFinderPattern.getWidth();
    int BLHeight = m_BLFinderPattern.getHeight();
    
    if (abs(ULWidth - URWidth) > variance ||
        abs(ULWidth - BLWidth) > variance ||
        abs(URWidth - BLWidth) > variance)
    {
        return false;
    }
    
    if (abs(ULHeight - URHeight) > variance ||
        abs(ULHeight - BLHeight) > variance ||
        abs(URHeight - BLHeight) > variance)
    {
        return false;
    }
    
    m_gridWidthStep  = ULWidth  / EXTERNAL_RECT_RATIO;
    m_gridHeightStep = ULHeight / EXTERNAL_RECT_RATIO;
    return true;
}

bool QRCodeDetector::checkBasisPoints()
{
    int variance = m_ULFinderPattern.getWidth() / 1;
    printf("VARIANCE - %d\n", variance);
    
    if (abs(m_ULBasisPoint.y - m_URBasisPoint.y) > variance ||
        abs(m_ULBasisPoint.x - m_BLBasisPoint.x) > variance)
    {
        return false;
    }
    
    return true;
}

void QRCodeDetector::calculateQRMatrixSize(int tempSize)
{
    int variance = 2;
    
    if (abs(tempSize - QR_SIZE_VERSION_1) < variance)
    {
        m_QRMatrixSize = QR_SIZE_VERSION_1;
        return;
    }
    if (abs(tempSize - QR_SIZE_VERSION_2) < variance)
    {
        m_QRMatrixSize = QR_SIZE_VERSION_2;
        return;
    }
    if (abs(tempSize - QR_SIZE_VERSION_3) < variance)
    {
        m_QRMatrixSize = QR_SIZE_VERSION_3;
        return;
    }
    if (abs(tempSize - QR_SIZE_VERSION_4) < variance)
    {
        m_QRMatrixSize = QR_SIZE_VERSION_4;
        return;
    }
    m_QRMatrixSize = -1;
}

void QRCodeDetector::alignQRCodeRatio()
{
    int codeWidth  = m_URBasisPoint.x - m_ULBasisPoint.x;
    int codeHeight = m_BLBasisPoint.y - m_ULBasisPoint.y;
    
    printf("W: %d, H: %d\n", codeWidth, codeHeight);
    
    m_gridWidthStep = (codeWidth / m_QRMatrixSize) + 1;
    printf("WidthStep: %d\n", m_gridWidthStep);
    
    while (codeWidth  % m_gridWidthStep != 0 ||
           codeWidth  / m_gridWidthStep != m_QRMatrixSize)
    {
        codeWidth++;
    }

    m_gridHeightStep = (codeHeight / m_QRMatrixSize) + 1;
    printf("HeightStep: %d\n", m_gridHeightStep);
    
    while (codeHeight  % m_gridHeightStep != 0 ||
           codeHeight /  m_gridHeightStep != m_QRMatrixSize)
    {
        codeHeight++;
    }
    
    printf("W: %d, H: %d\n", codeWidth, codeHeight);
    
    m_QRCanvasWidth  = codeWidth;
    m_QRCanvasHeight = codeHeight;
}

void QRCodeDetector::fillQRMatrix()
{
    m_QRMatrix = new int* [m_QRMatrixSize];
    
    for (int i = 0; i < m_QRMatrixSize; i++)
    {
        m_QRMatrix[i] = new int [m_QRMatrixSize];
    }
    
    for (int i = 0; i < m_QRMatrixSize; i++)
    {
        for (int j = 0; j < m_QRMatrixSize; j++)
        {
            if (m_frameAligned.at<uchar>(i * m_gridHeightStep + m_gridHeightStep / 2,
                                         j * m_gridWidthStep  + m_gridWidthStep  / 2)
                == BLACK_PIXEL)
            {
                m_QRMatrix[i][j] = 1;
                m_frameAligned.at<uchar>(i * m_gridHeightStep + m_gridHeightStep / 2,
                                         j * m_gridWidthStep  + m_gridWidthStep  / 2) = 255;
            }
            else
            {
                m_QRMatrix[i][j] = 0;
            }
        }
    }
    
}

#pragma mark QRCodeDetector - Basis point methods

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

#pragma mark QRCodeDetector - Rotated rect methods

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

#pragma mark QRCodeDetector - Show debug info

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
    for (int i = 0; i < m_QRMatrixSize + 1; i++)
    {
        for (int j = 0; j < m_QRMatrixSize + 1; j++)
        {
            showPoint(cv::Point2f(m_ULBasisPoint.x + i * m_gridWidthStep,
                                  m_ULBasisPoint.y + j * m_gridHeightStep),
                      cv::Scalar(255, 255, 255));
        }
    }
}

void QRCodeDetector::printQRMatrix()
{
    for (int i = 0; i < m_QRMatrixSize; i++)
    {
        for (int j = 0; j < m_QRMatrixSize; j++)
        {
            printf("%d ", m_QRMatrix[i][j]);
        }
        printf("\n");
    }
}
