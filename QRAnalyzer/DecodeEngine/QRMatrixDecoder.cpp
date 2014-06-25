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

#pragma mark QRMatrixDecoder - Base API

void QRMatrixDecoder::setQRMatrix(int** matrix, int size)
{
    m_QRMatrixSize = size;
    m_QRMatrix     = matrix;
}

void QRMatrixDecoder::decodeQRMatrix()
{
    readSystemInfo();
    
//    m_codeMask = 3;
//    m_QRMatrix[m_QRMatrixSize - 1][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 1][m_QRMatrixSize - 2] = 1;
//    m_QRMatrix[m_QRMatrixSize - 2][m_QRMatrixSize - 1] = 1;
//    m_QRMatrix[m_QRMatrixSize - 2][m_QRMatrixSize - 2] = 1;
    
    readHeader();
}

#pragma mark QRMatrixDecoder - Read QR data

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

void QRMatrixDecoder::readHeader()
{
    uchar mode = 0;
    
    for (int i = 0; i < HEADER_LENGTH / 2; i++)
    {
        int readRowIndex = m_QRMatrixSize - 1 - i;
        int readCollIndex = m_QRMatrixSize - 1;
        mode |= applyMask(m_QRMatrix[readRowIndex][readCollIndex] << (HEADER_LENGTH - (i * 2 + 1)),
                          readRowIndex,
                          readCollIndex);
        
        readCollIndex = m_QRMatrixSize - 2;
        mode |= applyMask(m_QRMatrix[readRowIndex][readCollIndex] << (HEADER_LENGTH - (i * 2 + 2)),
                          readRowIndex,
                          readCollIndex);
    }
    
    m_QRCodeMode = mode;
    printf("QR MODE: %d\n", mode);
}

#pragma mark QRMatrixDecoder - Utility methods

uchar QRMatrixDecoder::applyMask(uchar value, int x, int y)
{
    if (getMask(x, y) == 0)
    {
        return (~value) & AFTER_INVERT_MASK;
    }
    else
    {
        return value;
    }
}

int QRMatrixDecoder::getMask(int x, int y)
{
    switch (m_codeMask)
    {
        case 0:
        {
            return MASK_0(x, y);
        }
        case 1:
        {
            return MASK_1(x, y);
        }
        case 2:
        {
            return MASK_2(x, y);
        }
        case 3:
        {
            return MASK_3(x, y);
        }
        case 4:
        {
            return MASK_4(x, y);
        }
        case 5:
        {
            return MASK_5(x, y);
        }
        case 6:
        {
            return MASK_6(x, y);
        }
        case 7:
        {
            return MASK_7(x, y);
        }
        default:
        {
            return -1;
        }
    }
}