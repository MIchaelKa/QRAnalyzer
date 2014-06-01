//
//  RootTabBarController.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 31/05/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "RootTabBarController.h"

#import "MediaCaptureViewController.h"
#import "SettingsTableViewController.h"

@interface RootTabBarController ()

@end

@implementation RootTabBarController

//static const int LRVC_INDEX = 0;
static const int MCVC_INDEX = 1;
static const int STVC_INDEX = 2;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setSelectedIndex: MCVC_INDEX];
    
    MediaCaptureViewController* mcvc;
    SettingsTableViewController* stvc;
    
    if ([[self viewControllers][MCVC_INDEX] isKindOfClass: [MediaCaptureViewController class]])
    {
        mcvc = (MediaCaptureViewController*)[self viewControllers][MCVC_INDEX];
        
        if ([[self viewControllers][STVC_INDEX] isKindOfClass: [UINavigationController class]])
        {
            UINavigationController* nvc = (UINavigationController*)[self viewControllers][STVC_INDEX];
            
            if ([[nvc viewControllers][0] isKindOfClass: [SettingsTableViewController class]])
            {
                stvc = (SettingsTableViewController*)[nvc viewControllers][0];
                stvc.delegate = mcvc;
            }
        }
    }    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
