//
//  MediaCaptureViewController.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 31/05/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "MediaCaptureViewController.h"

#import <AVFoundation/AVFoundation.h>
#import "FocusView.h"

typedef unsigned char RAW_COLOR;

@interface MediaCaptureViewController () <AVCaptureVideoDataOutputSampleBufferDelegate>

// Session management.
@property (nonatomic) AVCaptureSession *captureSession;
@property (nonatomic) AVCaptureVideoPreviewLayer *captureVideoPreviewLayer;

// For storing captured frame
@property (nonatomic, strong) NSData *captureBuffer;
@property (nonatomic) NSUInteger captureFrameWidth;
@property (nonatomic) NSUInteger captureFrameHeight;
@property (nonatomic) NSUInteger captureFrameBytesPerRow;

// Views
@property (nonatomic, strong) FocusView* focusView;

@end


@implementation MediaCaptureViewController

static const int PIXEL_SIZE_IN_BYTES = 4;
static const int AREA_SIZE = 6;

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([[segue identifier] isEqualToString: @"Decode result"])
    {
        NSLog(@"Decode result segue!");
        UINavigationController* nc = segue.destinationViewController;
        QRDecodeResultViewController* drvc = [nc viewControllers][0];
        drvc.delegate = self;
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self setupCaptureSession];
    [self setupVeiws];
    [self setNeedsStatusBarAppearanceUpdate];

}

- (void) setupVeiws
{
    self.focusView = [[FocusView alloc] initAtCenter: self.view.center];
    [self.view addSubview: self.focusView];
}

- (UIStatusBarStyle) preferredStatusBarStyle
{
    return UIStatusBarStyleLightContent;
}

- (void) setupCaptureSession
{
    //Setup capture input
    AVCaptureDevice *videoDevice = [AVCaptureDevice defaultDeviceWithMediaType: AVMediaTypeVideo];
    AVCaptureDeviceInput *captureInput = [AVCaptureDeviceInput deviceInputWithDevice: videoDevice
                                                                               error: nil];
    
    //Setup capture output
    AVCaptureVideoDataOutput *captureOutput = [AVCaptureVideoDataOutput new];
    
    NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
    NSNumber* value = [NSNumber numberWithUnsignedInt: kCVPixelFormatType_32BGRA];
    NSDictionary* videoSettings = [NSDictionary dictionaryWithObject: value forKey: key];
    
    [captureOutput setVideoSettings: videoSettings];
    
    //Set the data output’s delegate
    dispatch_queue_t videoDataOutputQueue = dispatch_queue_create("VideoDataOutputQueue", NULL);
    [captureOutput setSampleBufferDelegate: self
                                     queue: videoDataOutputQueue];
    
    //Setup capture session
    self.captureSession = [AVCaptureSession new];
    self.captureSession.sessionPreset = AVCaptureSessionPresetMedium;
    
    if ([self.captureSession canAddInput: captureInput]) {
        [self.captureSession addInput: captureInput];
    }
    
    if ([self.captureSession canAddOutput: captureOutput]) {
        [self.captureSession addOutput: captureOutput];
    }
    
    //Showing the User What’s Being Recorded
    self.captureVideoPreviewLayer = [AVCaptureVideoPreviewLayer layerWithSession: self.captureSession];
    self.captureVideoPreviewLayer.frame = self.view.bounds;
    self.captureVideoPreviewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [self.view.layer addSublayer: self.captureVideoPreviewLayer];
    
    [self.captureSession startRunning];
}

- (void) captureOutput: (AVCaptureOutput *)captureOutput
 didOutputSampleBuffer: (CMSampleBufferRef)sampleBuffer
        fromConnection: (AVCaptureConnection *)connection
{
    // Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    
    self.captureFrameBytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    self.captureFrameWidth       = CVPixelBufferGetWidth(imageBuffer);
    self.captureFrameHeight      = CVPixelBufferGetHeight(imageBuffer);
    
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    NSUInteger sizeOfBuffer = self.captureFrameBytesPerRow * self.captureFrameHeight;
    
    self.captureBuffer = [NSData dataWithBytes: baseAddress
                                        length: sizeOfBuffer];
    
    if ([self isRedColor]) {
        NSLog(@"RED!!!");
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self.captureSession stopRunning];
            [self performSegueWithIdentifier: @"Decode result" sender: self];
        });
    }
    
    //    NSLog(@"Capture Output - Bytes per row: %zu", bytesPerRow);
    //    NSLog(@"Capture Output - Width: %zu", width);
    //    NSLog(@"Capture Output - Height: %zu", height);
    
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}

- (BOOL) isRedColor
{
    UIColor* currentColor = [self colorAtCenter];
    
    CGFloat red   = 0.0;
    CGFloat green = 0.0;
    CGFloat blue  = 0.0;
    CGFloat alpha = 0.0;
    
    [currentColor getRed: &red
                   green: &green
                    blue: &blue
                   alpha: &alpha];
    
    if ((red   * 250) > 160 &&
        (green * 250) < 100 &&
        (blue  * 250) < 100)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

- (UIColor *) colorAtCenter
{
    return [self colorAtPoint:
            CGPointMake((self.captureFrameWidth / 2)  - (AREA_SIZE / 2),
                        (self.captureFrameHeight / 2) - (AREA_SIZE / 2))];
}


- (UIColor *) colorAtPoint: (CGPoint) point
{
    RAW_COLOR *base = (RAW_COLOR *)[self.captureBuffer bytes];
    RAW_COLOR *pixel = base +
    (int)round(point.y * self.captureFrameBytesPerRow) +
    (int)round(point.x * PIXEL_SIZE_IN_BYTES);
    
    int r = 0;
    int g = 0;
    int b = 0;
    
    for ( int i = 0; i < AREA_SIZE; i++)
    {
        for ( int j = 0; j < AREA_SIZE; j++)
        {
            r += pixel[2];
            g += pixel[1];
            b += pixel[0];
            
            pixel += PIXEL_SIZE_IN_BYTES;
            //NSLog(@"r = %d, g = %d, b = %d", r, g, b);
        }
        pixel += self.captureFrameBytesPerRow - AREA_SIZE * PIXEL_SIZE_IN_BYTES;
    }
    
    return [UIColor colorWithRed: (r / (AREA_SIZE * AREA_SIZE)) / 255.0
                           green: (g / (AREA_SIZE * AREA_SIZE)) / 255.0
                            blue: (b / (AREA_SIZE * AREA_SIZE)) / 255.0
                           alpha: 1.0f];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - SettingsTableViewControllerDelegate

- (void)showFocusView;
{
    if (![self.focusView superview]) {
        [self.view addSubview: self.focusView];
    }
}

- (void)hideFocusView
{
    if ([self.focusView superview]) {
        [self.focusView removeFromSuperview];
    }
}

#pragma mark - QRDecodeResultViewControllerDelegate

- (void)continueSession
{
    [self.captureSession startRunning];    
}

@end
