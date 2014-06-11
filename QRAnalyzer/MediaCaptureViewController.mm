//
//  MediaCaptureViewController.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 31/05/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "MediaCaptureViewController.h"

#import <AVFoundation/AVFoundation.h>

#import "DecodeEngine/RawVideoFrame.h"
#import "DecodeEngine/QRCodeDetector.h"
#import "DecodeEngine/UIImage+OpenCV.h"

#import "FocusView.h"

typedef unsigned char RAW_COLOR;

@interface MediaCaptureViewController () <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    QRCodeDetector* codeDetector;
}

// Session management.
@property (nonatomic) AVCaptureSession *captureSession;
@property (nonatomic) AVCaptureVideoPreviewLayer *captureVideoPreviewLayer;

// Views
@property (nonatomic, strong) FocusView* focusView;
@property (nonatomic, strong) UIImage* resultImage;

@end


@implementation MediaCaptureViewController


- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([[segue identifier] isEqualToString: @"Decode result"])
    {
        NSLog(@"Decode result segue!");
        UINavigationController* nc = segue.destinationViewController;
        QRDecodeResultViewController* drvc = [nc viewControllers][0];
        drvc.delegate = self;
        [drvc updateResultView: self.resultImage];
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self setupCaptureSession];
    [self setupVeiws];
    [self setNeedsStatusBarAppearanceUpdate];
    
    codeDetector = new QRCodeDetector();
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
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

#pragma mark - AVCaptureVideoDataOutputSampleBufferDelegate

- (void) captureOutput: (AVCaptureOutput *)captureOutput
 didOutputSampleBuffer: (CMSampleBufferRef)sampleBuffer
        fromConnection: (AVCaptureConnection *)connection
{
    // Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    CVPixelBufferLockBaseAddress(imageBuffer, 0);

    RawVideoFrame frame((int)CVPixelBufferGetWidth(imageBuffer),
                        (int)CVPixelBufferGetHeight(imageBuffer),
                        (int)CVPixelBufferGetBytesPerRow(imageBuffer),
                        CVPixelBufferGetBaseAddress(imageBuffer));
    
    cv::Mat processFrame = frame.toCVMat();
    if (codeDetector->detectRedColor(processFrame)) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self.captureSession stopRunning];
            self.resultImage = [UIImage fromCVMat: processFrame];
            [self performSegueWithIdentifier: @"Decode result" sender: self];
        });
    }
    
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
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
