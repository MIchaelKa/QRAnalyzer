//
//  QRMatrixDecoder.cpp
//  QRAnalyzer
//
//  Created by Michael Kalinin on 21/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#include "QRMatrixDecoder.h"

QRMatrixDecoder::QRMatrixDecoder()
{
    
}

void QRMatrixDecoder::setQRMatrix(int** matrix, int size)
{
    m_QRMatrixSize = size;
    m_QRMatrix     = matrix;
}

void QRMatrixDecoder::decodeQRMatrix()
{
    readSystemInfo();
}

void QRMatrixDecoder::readSystemInfo()
{
    uchar systemInfo = 0;
    
    for (int i = 0; i < SYSTEM_INFO_LENGTH; i++)
    {
        systemInfo |= m_QRMatrix[SYSTEM_INFO_INDEX][i] << (SYSTEM_INFO_LENGTH - (i + 1));
    }
    
    systemInfo ^= SYSTEM_INFO_MASK;
    
    printf("SYSTEM INFO: %d\n", systemInfo);
    
    m_errorLevel = systemInfo & SYSTEM_INFO_ERR_MASK;
    m_codeMask   = systemInfo & SYSTEM_INFO_CODE_MASK_MASK;
    
    printf("ERR LEVEL: %d\n", m_errorLevel);
    printf("CODE MASK: %d\n", m_codeMask);
}