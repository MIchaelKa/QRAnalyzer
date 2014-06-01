//
//  MediaCaptureViewController.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 31/05/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "SettingsTableViewController.h"
#import "QRDecodeResultViewController.h"

@interface MediaCaptureViewController : UIViewController <SettingsTableViewControllerDelegate,
                                                          QRDecodeResultViewControllerDelegate>

@end
