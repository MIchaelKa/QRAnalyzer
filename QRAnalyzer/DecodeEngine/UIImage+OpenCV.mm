//
//  UIImage+OpenCV.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 12/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "UIImage+OpenCV.h"

@implementation UIImage (OpenCV)

+ (cv::Mat)toCVMat:(UIImage*)image
{
    // (1) Get image dimensions
    CGFloat cols = image.size.width;
    CGFloat rows = image.size.height;
    
    // (2) Create OpenCV image container, 8 bits per component, 4 channels
    cv::Mat cvMat(rows, cols, CV_8UC4);
    
    // (3) Create CG context and draw the image
    CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,
                                                    cols,
                                                    rows,
                                                    8,
                                                    cvMat.step[0],
                                                    CGImageGetColorSpace(image.CGImage),
                                                    kCGImageAlphaNoneSkipLast |
                                                    kCGBitmapByteOrderDefault);
    
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), image.CGImage);
    CGContextRelease(contextRef);
    
    // (4) Return OpenCV image container reference
    return cvMat;
}

- (cv::Mat)toCVMat
{
    return [UIImage toCVMat:self];
}

+ (UIImage*)fromCVMat:(const cv::Mat&)cvMat
{
    NSData *data = [NSData dataWithBytes: cvMat.data
                                  length: cvMat.elemSize() * cvMat.total()];
    // (1) Construct the correct color space
    CGColorSpaceRef colorSpace;
    if ( cvMat.channels() == 1 ) {
        colorSpace = CGColorSpaceCreateDeviceGray();
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    
    // (2) Create image data reference
    //CFDataRef data = CFDataCreate(kCFAllocatorDefault, cvMat.data, (cvMat.elemSize() * cvMat.total()));
    
    // (3) Create CGImage from cv::Mat container
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data);
    CGImageRef imageRef = CGImageCreate(cvMat.cols,
                                        cvMat.rows,
                                        8,
                                        8 * cvMat.elemSize(),
                                        cvMat.step[0],
                                        colorSpace,
                                        //kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little,
                                        kCGImageAlphaNone | kCGBitmapByteOrderDefault,
                                        provider,
                                        NULL,
                                        false,
                                        kCGRenderingIntentDefault);
    
    // (4) Create UIImage from CGImage
    UIImage * finalImage = [UIImage imageWithCGImage: imageRef];
    
    // (5) Release the references
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    //CFRelease(data);
    CGColorSpaceRelease(colorSpace);
    
    // (6) Return the UIImage instance
    return finalImage;
}

@end
