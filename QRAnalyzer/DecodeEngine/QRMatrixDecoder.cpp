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