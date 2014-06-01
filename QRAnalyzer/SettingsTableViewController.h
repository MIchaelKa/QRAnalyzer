//
//  SettingsTableViewController.h
//  QRAnalyzer
//
//  Created by Michael Kalinin on 31/05/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol SettingsTableViewControllerDelegate <NSObject>

- (void)showFocusView;
- (void)hideFocusView;

@end

@interface SettingsTableViewController : UITableViewController

@property (nonatomic, weak) id <SettingsTableViewControllerDelegate> delegate;

@end
