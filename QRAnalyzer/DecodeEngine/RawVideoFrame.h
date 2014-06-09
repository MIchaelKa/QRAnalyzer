//
//  RawVideoFrame.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 10/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef __QRAnalyzer__RawVideoFrame__
#define __QRAnalyzer__RawVideoFrame__

class RawVideoFrame
{
public:
    RawVideoFrame(int width, int height, int stride, void* data);
    
    cv::Mat toCVMat();
    
private:
    
    int m_width;
    int m_height;
    int m_stride;
    
    void* m_data;
};

#endif /* defined(__QRAnalyzer__RawVideoFrame__) */
