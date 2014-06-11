//
//  UIImage+OpenCV.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 12/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (OpenCV)

#pragma mark -
#pragma mark Generate UIImage from cv::Mat
+ (UIImage*)fromCVMat:(const cv::Mat&)cvMat;

#pragma mark -
#pragma mark Generate cv::Mat from UIImage
+ (cv::Mat)toCVMat:(UIImage*)image;
- (cv::Mat)toCVMat;

@end
