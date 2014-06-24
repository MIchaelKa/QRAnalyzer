//
//  QRCodeMasks.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 25/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#ifndef QRAnalyzer_QRCodeMasks_h
#define QRAnalyzer_QRCodeMasks_h


// x - coll, y - row

#define MASK_0(x,y) ((x) + (y)) % 2
#define MASK_1(x,y) (y) % 2
#define MASK_2(x,y) (x) % 3
#define MASK_3(x,y) ((x) + (y)) % 3
#define MASK_4(x,y) ((x) / 3 + (y) /2 ) % 2
#define MASK_5(x,y) ((x) * (y)) % 2 + ((x) * (y)) % 3
#define MASK_6(x,y) (((x) * (y)) % 2 + ((x) * (y)) % 3) % 2
#define MASK_7(x,y) (((x) * (y)) % 3 + ((x) + (y)) % 2) % 2

#endif
