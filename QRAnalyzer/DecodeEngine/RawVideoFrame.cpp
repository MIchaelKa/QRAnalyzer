//
//  RawVideoFrame.cpp
//  QRAnalyzer
//
//  Created by Michael Kalinin on 10/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#include "RawVideoFrame.h"

RawVideoFrame::RawVideoFrame(int width, int height, int stride, void* data)
{
    m_width  = width;
    m_height = height;
    m_stride = stride;
    m_data   = data;    
}

cv::Mat RawVideoFrame::toCVMat()
{
    return cv::Mat(m_height, m_width, CV_8UC4, m_data, m_stride);
}