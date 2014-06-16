//
//  QRCodeDetector.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 10/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef __QRAnalyzer__QRCodeDetector__
#define __QRAnalyzer__QRCodeDetector__

using namespace std;

enum FinderPatternSections
{
    FIRST_BLACK = 0,
    SECOND_WHITE = 1,
    THIRD_BLACH = 2,
    FOURTH_WHITE = 3,
    FIFTH_BLACK = 4,
    NUMBER_OF_SECTION = 5
};

enum PixelValue
{
    BLACK_PIXEL = 0,
    WHITE_PIXEL = 250,
    INVALID_PIXEL = -1
};

#define FINDER_PATTERNS  3
#define FINDER_SECTIONS  3
#define MIN_COUNTUR_AREA 300

struct FinderPattern
{
    cv::RotatedRect externalRect;
    cv::RotatedRect middleRect;
    cv::RotatedRect innerRect;
};

class QRCodeDetector
{
public:
    QRCodeDetector();
    
    bool detectQRCode(cv::Mat& frame);
    
private:
    //methods
    bool findQRCodePosition(cv::Mat& mat);
    bool rectIsContainInnerRect(cv::RotatedRect& externalRect,
                                cv::RotatedRect& innerRect);
    bool rectsIsEqual(cv::RotatedRect& firstRect,
                      cv::RotatedRect& secondRect);
    void addNewFinderPatternRect(cv::RotatedRect& rect);


    
    bool identifyFinderPatterns(cv::Mat& mat);
    bool checkRatio(int pixelCount[]);
    void clearPixelCount();
    PixelValue invertPixelValue(PixelValue value);
    
    //variables
    cv::Mat m_frameOriginal;
    cv::Mat m_frameThreshold;
    
    vector<cv::RotatedRect> m_finderPatternRects;
    
};

#endif /* defined(__QRAnalyzer__QRCodeDetector__) */
