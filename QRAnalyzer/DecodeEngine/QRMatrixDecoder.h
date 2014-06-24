//
//  QRMatrixDecoder.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 21/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef __QRAnalyzer__QRMatrixDecoder__
#define __QRAnalyzer__QRMatrixDecoder__

#include "DecodeEngineDefines.h"
#include "QRCodeMasks.h"

class QRMatrixDecoder
{
public:
    QRMatrixDecoder();
    
    void setQRMatrix(int** matrix, int size);
    void decodeQRMatrix();
    
private:
    // Read QR data
    void readSystemInfo();
    
    // Utility methods
    int  applyMask(int x, int y);
    
    // System Info
    uchar m_errorLevel;
    uchar m_codeMask;
    
    // QR Code Matrix
    int   m_QRMatrixSize;
    int** m_QRMatrix;
    
};

#endif /* defined(__QRAnalyzer__QRMatrixDecoder__) */
