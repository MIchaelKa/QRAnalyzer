//
//  RawCapturedFrame.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 03/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "RawCapturedFrame.h"

@implementation RawCapturedFrame

- (id) initWithBuffer: (NSData *)   buffer
                width: (NSUInteger) width
               height: (NSUInteger) height
          bytesPerRow: (NSUInteger) bytesPerRow
{
    self = [super init];
    if (self) {
        self.buffer = buffer;
        self.width = width;
        self.height = height;
        self.bytesPerRow = bytesPerRow;
    }
    return self;
}

- (void) updateWithImageBuffer: (CVImageBufferRef) imageBuffer
{
    self.bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    self.width       = CVPixelBufferGetWidth(imageBuffer);
    self.height      = CVPixelBufferGetHeight(imageBuffer);
    
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    NSUInteger sizeOfBuffer = self.bytesPerRow * self.height;
    
    self.buffer = [NSData dataWithBytes: baseAddress
                                 length: sizeOfBuffer];
}

@end
