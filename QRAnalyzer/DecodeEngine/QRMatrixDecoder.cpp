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
    
    m_currentRowIndex = m_QRMatrixSize - 1;
    m_currentColIndex = m_QRMatrixSize - 1;
    m_currentDirection = DOWN_UP;
}

void QRMatrixDecoder::decodeQRMatrix()
{
    readSystemInfo();
    
//    m_codeMask = 3;
//    
//    m_QRMatrix[m_QRMatrixSize - 1][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 1][m_QRMatrixSize - 2] = 1;
//    
//    m_QRMatrix[m_QRMatrixSize - 2][m_QRMatrixSize - 1] = 1;
//    m_QRMatrix[m_QRMatrixSize - 2][m_QRMatrixSize - 2] = 1;
//    
//    m_QRMatrix[m_QRMatrixSize - 3][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 3][m_QRMatrixSize - 2] = 0;
//    
//    m_QRMatrix[m_QRMatrixSize - 4][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 4][m_QRMatrixSize - 2] = 1;
//    
//    m_QRMatrix[m_QRMatrixSize - 5][m_QRMatrixSize - 1] = 1;
//    m_QRMatrix[m_QRMatrixSize - 5][m_QRMatrixSize - 2] = 0;
//    
//    m_QRMatrix[m_QRMatrixSize - 6][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 6][m_QRMatrixSize - 2] = 0;
//    
//    m_QRMatrix[m_QRMatrixSize - 7][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 7][m_QRMatrixSize - 2] = 0;
//    
//    m_QRMatrix[m_QRMatrixSize - 8][m_QRMatrixSize - 1] = 1;
//    m_QRMatrix[m_QRMatrixSize - 8][m_QRMatrixSize - 2] = 1;
//    
//    m_QRMatrix[m_QRMatrixSize - 9][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 9][m_QRMatrixSize - 2] = 1;
//    
//    m_QRMatrix[m_QRMatrixSize - 10][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 10][m_QRMatrixSize - 2] = 1;
//    
//    m_QRMatrix[m_QRMatrixSize - 11][m_QRMatrixSize - 1] = 1;
//    m_QRMatrix[m_QRMatrixSize - 11][m_QRMatrixSize - 2] = 0;
//    
//    m_QRMatrix[m_QRMatrixSize - 12][m_QRMatrixSize - 1] = 0;
//    m_QRMatrix[m_QRMatrixSize - 12][m_QRMatrixSize - 2] = 0;
    
    
    readHeader();
    readPacketNumber();
    
    switch (m_QRCodeMode)
    {
        case QR_MODE_NUMERIC:
        {
            readNumericData();
            break;
        }
        case QR_MODE_ALPHANUMERIC:
        {
            break;
        }
        case QR_MODE_BYTE:
        {
            readByteData();
            break;
        }
    }

}

#pragma mark QRMatrixDecoder - Read QR data

void QRMatrixDecoder::readNumericData()
{
    m_result = new char[m_packetNumber + 1];
    
    int result = 0;
    
    int blockNumber = m_packetNumber / 3 + 1;
    
    for (int i = 0; i < blockNumber; i++)
    {
        readBlock(m_blockLength, result);
    }
    if (m_packetNumber == 1)
    {
        result >>= 6;
        result &= 0x0F;
    }
    else if (m_packetNumber == 2)
    {
        result >>= 3;
        result &= 0x7F;
    }
    m_numericResult = result;
    sprintf(m_result, "%d\n", m_numericResult);
    
    printf("RESULT: %s\n", m_result);
    
}

void QRMatrixDecoder::readByteData()
{
    m_result = new char[m_packetNumber + 1];
    int temp = 0;
    
    m_result[m_packetNumber] = '\0';
    
    for (int i = 0; i < m_packetNumber; i++)
    {
        readBlock(m_blockLength, temp);
        m_result[i] = temp;
        temp = 0;
    }
    
    printf("RESULT: %s\n", m_result);

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

void QRMatrixDecoder::readHeader()
{
    int mode = 0;
    
    readBlock(HEADER_LENGTH, mode);
    
    m_QRCodeMode  = mode;
    m_blockLength = getBlockLength(m_QRCodeMode); // return false!!!!
    
    printf("QR MODE: %d\n", mode);
    printf("Block Length: %d\n", m_blockLength);
}

void QRMatrixDecoder::readPacketNumber()
{
    int number = 0;
    
    readBlock(m_blockLength, number);
    
    m_packetNumber = number;
    printf("Packet Number: %d\n", number);
}

void QRMatrixDecoder::readBlock(int blockLength, int& destination)
{
    int block = 0;
    
    for (int i = 0; i < blockLength / 2; i++)
    {
        block |= applyMask(m_QRMatrix[m_currentRowIndex][m_currentColIndex],
                           m_currentColIndex,
                           m_currentRowIndex) << (blockLength - (i * 2 + 1));
        
        block |= applyMask(m_QRMatrix[m_currentRowIndex][m_currentColIndex - 1],
                           m_currentColIndex - 1,
                           m_currentRowIndex) << (blockLength - (i * 2 + 2));
        
        if (m_currentDirection == DOWN_UP)
        {
            m_currentRowIndex--;
        }
        else
        {
            m_currentRowIndex++;
        }
        
        if (m_currentRowIndex == SYSTEM_INFO_INDEX &&
            m_currentColIndex > m_QRMatrixSize - SYSTEM_INFO_INDEX)
        {
            m_currentDirection = UP_DOWN;
            m_currentColIndex -= 2;
            m_currentRowIndex++;
        }
    }
    destination = block;
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

int QRMatrixDecoder::getBlockLength(int mode)
{
    switch (mode)
    {
        case QR_MODE_NUMERIC:
        {
            return BLOCK_LENGTH_NUMERIC;
        }
        case QR_MODE_ALPHANUMERIC:
        {
            return BLOCK_LENGTH_ALPHANUMERIC;
        }
        case QR_MODE_BYTE:
        {
            return BLOCK_LENGTH_BYTE ;
        }
        default:
        {
            return -1;
        }
    }    
}

int QRMatrixDecoder::getNumericResult()
{
    return m_numericResult;
}

char* QRMatrixDecoder::getResult()
{
    return m_result;
}