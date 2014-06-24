//
//  DecodeEngineDefines.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 20/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef QRAnalyzer_DecodeEngineDefines_h
#define QRAnalyzer_DecodeEngineDefines_h


#define FINDER_PATTERNS  3
#define FINDER_SECTIONS  3
#define MIN_COUNTUR_AREA 100 //to settings

enum PixelValue
{
    BLACK_PIXEL = 0,
    WHITE_PIXEL = 250,
    INVALID_PIXEL = -1
};

enum FinderPatternRatio
{
    EXTERNAL_RECT_RATIO = 7,
    MIDDLE_RECT_RATIO   = 5,
    INNER_RECT_RATIO    = 3
};

struct FinderPattern
{
    cv::RotatedRect externalRect;
    cv::RotatedRect middleRect;
    cv::RotatedRect innerRect;
};

enum QRVersionSize
{
    QR_SIZE_VERSION_1 = 21,
    QR_SIZE_VERSION_2 = 25,
    QR_SIZE_VERSION_3 = 29,
    QR_SIZE_VERSION_4 = 33,
};

enum QRElementSystemInfo
{
    // Index
    SYSTEM_INFO_INDEX            = 8,
    // Lengthes
    SYSTEM_INFO_ERR_LEVEL        = 2,
    SYSTEM_INFO_CODE_MASK        = 3,
    SYSTEM_INFO_LENGTH           = 5,
    // Masks
    SYSTEM_INFO_MASK             = 0x15, // 00010101
    SYSTEM_INFO_ERR_MASK         = 0x18, // 00011000
    SYSTEM_INFO_CODE_MASK_MASK   = 0x07, // 00000111
};

#endif
