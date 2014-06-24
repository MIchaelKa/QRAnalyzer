//
//  CorrectFinderPattern.cpp
//  QRAnalyzer
//
//  Created by Michael Kalinin on 20/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#include "CorrectFinderPattern.h"

CorrectFinderPattern::CorrectFinderPattern()
{
    
}

void CorrectFinderPattern::setCVMat(cv::Mat mat)
{
    m_frameThreshold = mat;
}

void CorrectFinderPattern::initWithRotatedRect(cv::RotatedRect rect)
{
    cv::Point2f rect_points[4];
    rect.points(rect_points);
    
    for (int i = 0; i < 4; i++)
    {
        if (rect_points[i].x < rect.center.x && rect_points[i].y < rect.center.y)
        {
            m_ULPoint = rect_points[i];
        }
        else
        if (rect_points[i].x > rect.center.x && rect_points[i].y < rect.center.y)
        {
            m_URPoint = rect_points[i];
        }
        else
        if (rect_points[i].x < rect.center.x && rect_points[i].y > rect.center.y)
        {
            m_BLPoint = rect_points[i];
        }
        else
        {
            m_BRPoint = rect_points[i];
        }
    }    
}

void CorrectFinderPattern::correctPoints()
{
    correctULPoint();
    correctURPoint();
    correctBLPoint();
    correctBRPoint();
    
    m_step = round(m_URPoint.x - m_ULPoint.x) / EXTERNAL_RECT_RATIO;
}

int CorrectFinderPattern::getWidth()
{
    return m_URPoint.x - m_ULPoint.x;
}

int CorrectFinderPattern::getHeight()
{
    return m_BRPoint.y - m_ULPoint.y;
}

void CorrectFinderPattern::correctULPoint()
{
    int x = m_ULPoint.x;
    int y = m_ULPoint.y;
    
    int searchRange = 10;
    
    for (int i = 1; i < searchRange; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            if (m_frameThreshold.at<uchar>(y + i - j, x + j) == BLACK_PIXEL)
            {
                m_ULPoint.x = x + j;
                m_ULPoint.y = y + i - j;
                return;
            }
        }
    }
}

void CorrectFinderPattern::correctURPoint()
{
    int x = m_URPoint.x;
    int y = m_URPoint.y;
    
    int searchRange = 10;
    
    for (int i = 1; i < searchRange; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            if (m_frameThreshold.at<uchar>(y + i - j, x - j) == BLACK_PIXEL)
            {
                m_URPoint.x = x - j;
                m_URPoint.y = y + i - j;
                return;
            }
        }
    }
}

void CorrectFinderPattern::correctBLPoint()
{
    int x = m_BLPoint.x;
    int y = m_BLPoint.y;
    
    int searchRange = 10;
    
    for (int i = 1; i < searchRange; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            if (m_frameThreshold.at<uchar>(y - i + j, x + j) == BLACK_PIXEL)
            {
                m_BLPoint.x = x + j;
                m_BLPoint.y = y - i + j;
                return;
            }
        }
    }
    
}

void CorrectFinderPattern::correctBRPoint()
{
    
}

cv::Point2f CorrectFinderPattern::getULPoint()
{
    return m_ULPoint;
}

cv::Point2f CorrectFinderPattern::getURPoint()
{
    return m_URPoint;
}

cv::Point2f CorrectFinderPattern::getBLPoint()
{
    return m_BLPoint;
}

cv::Point2f CorrectFinderPattern::getBRPoint()
{
    return m_BRPoint;
}

int CorrectFinderPattern::getStep()
{
    return m_step;
}


