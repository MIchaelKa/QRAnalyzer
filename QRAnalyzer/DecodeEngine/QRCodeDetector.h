//
//  QRCodeDetector.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 10/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef __QRAnalyzer__QRCodeDetector__
#define __QRAnalyzer__QRCodeDetector__

#include "CorrectFinderPattern.h"
#include "DecodeEngineDefines.h"

using namespace std;

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
    
    // checkers and correction methods
    void deskewQRCanvas();
    void alignQRCodeRatio();
    void correctFinderPatterns();
    bool checkRatio();
    bool checkSize();
    bool checkBasisPoints();
    void calculateQRMatrixSize(int tempSize);
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
    cv::Point2f getURBasisPoint();    
    cv::Point2f getBLBasisPoint();
    
    int m_QRCanvasWidth;
    int m_QRCanvasHeight;
    
    // debug methods
    void showFinderPatterns();
    void showCorrectFinderPatterns();
    void showRotatedRect(cv::RotatedRect rotatedRect, cv::Scalar color);
    void showPoint(cv::Point2f point, cv::Scalar color);
    void showGrid();
    void printQRMatrix();

    
    // variables
    cv::Mat m_frameOriginal;
    cv::Mat m_frameThreshold;
    cv::Mat m_frameAligned;
    
    vector<cv::RotatedRect> m_finderPatternRects;
    vector<FinderPattern>   m_finderPatterns;
    
    cv::Point2f m_ULBasisPoint;
    cv::Point2f m_URBasisPoint;
    cv::Point2f m_BLBasisPoint;
    
    CorrectFinderPattern m_ULFinderPattern;
    CorrectFinderPattern m_URFinderPattern;
    CorrectFinderPattern m_BLFinderPattern;
    
    int m_gridWidthStep;
    int m_gridHeightStep;

    int   m_QRMatrixSize;
    int** m_QRMatrix;
    
};

#endif /* defined(__QRAnalyzer__QRCodeDetector__) */
