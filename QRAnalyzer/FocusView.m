//
//  FocusView.m
//  QRAnalyzer
//
//  Created by Michael Kalinin on 31/05/14.
//  Copyright (c) 2014 Michael. All rights reserved.
//

#import "FocusView.h"

@implementation FocusView

static const int FOCUS_VIEW_SIZE = 200;
static const int FOCUS_RECT_SIZE = 15;
static const int CORNER_RADIUS   = 25;
static const int LINE_WIDTH      = 12;

- (id)initAtCenter: (CGPoint)center
{
    CGRect frame;
    frame.origin = CGPointMake(center.x - FOCUS_VIEW_SIZE / 2,
                               center.y - FOCUS_VIEW_SIZE / 2);
    frame.size = CGSizeMake(FOCUS_VIEW_SIZE, FOCUS_VIEW_SIZE);
    return [self initWithFrame: frame];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = nil;
        self.opaque = NO;
        self.contentMode = UIViewContentModeRedraw;
    }
    return self;
}


// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect: (CGRect)rect
{
    [[UIColor colorWithRed: 255 / 255.0
                     green: 255 / 255.0
                      blue: 255 / 255.0
                     alpha: 100 / 255.0] setStroke];
    
    CGPoint arcCenter = CGPointMake(self.bounds.origin.x + CORNER_RADIUS + LINE_WIDTH / 2,
                                    self.bounds.origin.y + CORNER_RADIUS + LINE_WIDTH / 2);
    
    UIBezierPath* arc = [UIBezierPath bezierPathWithArcCenter: arcCenter
                                                       radius: CORNER_RADIUS
                                                   startAngle: M_PI
                                                     endAngle: 3 * M_PI / 2
                                                    clockwise: YES];
    [arc setLineWidth: LINE_WIDTH];
    [arc stroke]; // NW
    
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    // SW
    CGContextTranslateCTM(context, 0, self.bounds.size.height);
    CGContextRotateCTM(context, - M_PI / 2);
    [arc stroke];
    // SE
    CGContextTranslateCTM(context, 0, self.bounds.size.height);
    CGContextRotateCTM(context, - M_PI / 2);
    [arc stroke];
    // NE
    CGContextTranslateCTM(context, 0, self.bounds.size.height);
    CGContextRotateCTM(context, - M_PI / 2);
    [arc stroke];
    
    [[UIColor colorWithRed: 255 / 255.0
                     green: 255 / 255.0
                      blue: 255 / 255.0
                     alpha: 100 / 255.0] setFill];
    
    CGRect focusRect = CGRectMake(self.bounds.origin.x + self.bounds.size.width / 2 - FOCUS_RECT_SIZE / 2,
                                  self.bounds.origin.y + self.bounds.size.height / 2 - FOCUS_RECT_SIZE / 2,
                                  FOCUS_RECT_SIZE,
                                  FOCUS_RECT_SIZE);
    
    UIBezierPath* focus = [UIBezierPath bezierPathWithRoundedRect: focusRect
                                                     cornerRadius: 2];
    
    [focus fill];
}

@end
