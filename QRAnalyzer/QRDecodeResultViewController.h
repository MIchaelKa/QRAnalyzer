//
//  QRDecodeResultViewController.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 02/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol QRDecodeResultViewControllerDelegate <NSObject>

- (void)continueSession;

@end

@interface QRDecodeResultViewController : UIViewController

@property (nonatomic, weak) id<QRDecodeResultViewControllerDelegate> delegate;

- (IBAction)cancel: (id)sender;

@end
