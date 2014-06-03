//
//  QRDecodeResultViewController.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 02/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "QRDecodeResultViewController.h"

@interface QRDecodeResultViewController ()

@end

@implementation QRDecodeResultViewController

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
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)cancel: (id)sender
{
    [self dismissViewControllerAnimated: YES completion: ^{
        [self.delegate continueSession];
    }];    
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
