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

enum PixelValue
{
    BLACK_PIXEL = 0,
    WHITE_PIXEL = 250,
    INVALID_PIXEL = -1
};

#define FINDER_PATTERNS  3
#define FINDER_SECTIONS  3
#define MIN_COUNTUR_AREA 100 //to settings

struct FinderPattern
{
    cv::RotatedRect externalRect;
    cv::RotatedRect middleRect;
    cv::RotatedRect innerRect;
};

enum FinderPatternRatio
{
    EXTERNAL_RECT_RATIO = 7,
    MIDDLE_RECT_RATIO   = 5,
    INNER_RECT_RATIO    = 3
};

class QRCodeDetector
{
public:
    QRCodeDetector();
    
    bool detectQRCode(cv::Mat& frame);
    void setMinimumCodeSize(int size);
    
private:

    // methods
    bool identifyFinderPatterns(cv::Mat& mat);
    bool addToFinderPattern(FinderPattern& finderPattern,
                            cv::RotatedRect& rect);
    void addNewFinderPatternRect(cv::RotatedRect& rect);
    bool checkRatio();
    bool checkSize();
    bool checkBasisPoints();
    void fillQRMatrix();
    // to FinderPatternRect
    bool rectIsContainInnerRect(cv::RotatedRect& externalRect,
                                cv::RotatedRect& innerRect);
    bool rectsIsEqual(cv::RotatedRect& firstRect,
                      cv::RotatedRect& secondRect);
    bool rectsAtTheSameCenter(cv::RotatedRect& firstRect,
                              cv::RotatedRect& secondRect);
    
    // basis points
    cv::Point2f getULBasisPoint();
    void        correctULBasisPoint();
    
    cv::Point2f getURBasisPoint();
    void        correctURBasisPoint();
    
    cv::Point2f getBLBasisPoint();
    
    // debug methods
    void showFinderPatterns();
    void showRotatedRect(cv::RotatedRect rotatedRect, cv::Scalar color);
    void showPoint(cv::Point2f point);
    void showGrid();
    void printQRMatrix();

    
    // variables
    cv::Mat m_frameOriginal;
    cv::Mat m_frameThreshold;
    
    vector<cv::RotatedRect> m_finderPatternRects;
    vector<FinderPattern>   m_finderPatterns;
    
    cv::Point2f m_ULBasisPoint;
    cv::Point2f m_URBasisPoint;
    cv::Point2f m_BLBasisPoint;
    
    int m_gridStep;

    int   m_QRMatrixHeight;
    int   m_QRMatrixWidth;
    int** m_QRMatrix;

    
};

#endif /* defined(__QRAnalyzer__QRCodeDetector__) */
