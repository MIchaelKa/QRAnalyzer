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
    
    // Get methods
    int   getNumericResult();
    char* getResult();
    
private:
    // Read QR data
    void readNumericData();
    void readByteData();
    
    void readSystemInfo();
    void readHeader();
    void readPacketNumber();
    
    void readBlock(int  blockLength,
                   int& destination);
    
    // Utility methods
    uchar applyMask(uchar value, int x, int y);
    int   getMask(int x, int y);
    int   getBlockLength(int mode);

    // Variables
    int m_packetNumber;
    uchar m_blockLength;
    
    // System Info
    uchar m_errorLevel;
    uchar m_codeMask;
    
    // Header info
    uchar m_QRCodeMode;
    
    // QR Code Matrix
    int   m_QRMatrixSize;
    int** m_QRMatrix;
    int   m_currentRowIndex;
    int   m_currentColIndex;
    ReadingDirection m_currentDirection;
    
    // Results;
    int   m_numericResult;
    char* m_result;
    
    uchar m_testValue;
};

#endif /* defined(__QRAnalyzer__QRMatrixDecoder__) */
