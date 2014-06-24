//
//  QRMatrixDecoder.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 21/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef __QRAnalyzer__QRMatrixDecoder__
#define __QRAnalyzer__QRMatrixDecoder__

class QRMatrixDecoder
{
public:
    QRMatrixDecoder();
    
    void setQRMatrix(int** matrix, int size);
    
private:
    
    int   m_QRMatrixSize;
    int** m_QRMatrix;
    
};

#endif /* defined(__QRAnalyzer__QRMatrixDecoder__) */
