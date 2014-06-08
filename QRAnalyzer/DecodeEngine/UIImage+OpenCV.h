//
//  UIImage+OpenCV.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 08/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (OpenCV)

+ (UIImage*)fromCVMat:(const cv::Mat&)cvMat;

+ (cv::Mat)toCVMat:(UIImage*)image;
- (cv::Mat)toCVMat;

@end
