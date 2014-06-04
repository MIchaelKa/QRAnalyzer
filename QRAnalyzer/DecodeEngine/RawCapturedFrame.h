//
//  RawCapturedFrame.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 03/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RawCapturedFrame : NSObject

@property (nonatomic, strong) NSData *buffer;
@property (nonatomic) NSUInteger width;
@property (nonatomic) NSUInteger height;
@property (nonatomic) NSUInteger bytesPerRow;

- (id) initWithBuffer: (NSData *)   buffer
                width: (NSUInteger) width
               height: (NSUInteger) height
          bytesPerRow: (NSUInteger) bytesPerRow;

- (void) updateWithImageBuffer: (CVImageBufferRef) imageBuffer;


@end
