//
//  QRDecodeResultViewController.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 02/06/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "QRDecodeResultViewController.h"

@interface QRDecodeResultViewController ()

@property (weak, nonatomic) IBOutlet UIScrollView *testScrollView;
@property (strong, nonatomic) UIImageView* imageView;
@property (weak, nonatomic) IBOutlet UILabel *resultLable;

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

- (void) updateResultView: (UIImage *) image
{
    dispatch_async(dispatch_get_main_queue(), ^{
        self.imageView.image = image;
        [self.imageView sizeToFit];
        
        self.testScrollView.minimumZoomScale = 0.2;
        self.testScrollView.maximumZoomScale = 2.0;
        self.testScrollView.contentSize = image ? image.size : CGSizeZero;
        
        [self.testScrollView addSubview: self.imageView];
    });
}

- (void) updateResultLabel: (NSString *) result
{
    dispatch_async(dispatch_get_main_queue(), ^{
        self.resultLable.text = result;
    });
    
}

- (UIImageView *) imageView
{
    if (!_imageView) _imageView = [[UIImageView alloc] init];
    return _imageView;
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
