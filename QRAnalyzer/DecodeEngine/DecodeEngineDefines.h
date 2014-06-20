//
//  DecodeEngineDefines.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 20/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef QRAnalyzer_DecodeEngineDefines_h
#define QRAnalyzer_DecodeEngineDefines_h

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

#endif
