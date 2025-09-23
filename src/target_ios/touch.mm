//
//  touch.m
//  07-texturing
//
//  Created by apple on 2025-07-11.
//  Copyright © 2025 Apple. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <MetalKit/MetalKit.h>
#include <CoreGraphics/CoreGraphics.h>

void TouchHandler_handleTouch(int idx, float x, float y, int phase);

void ReloadFrameBuffers();

@interface CustomMTKView : MTKView {
    UITouch *_touchMap[12];
}
@end

UISelectionFeedbackGenerator* feedback = [[UISelectionFeedbackGenerator alloc] init];

void FeedbackGenerator() {
    [feedback prepare];
    [feedback selectionChanged];
}
@implementation CustomMTKView

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    FeedbackGenerator();
    [self processTouches:touches phase:0];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self processTouches:touches phase:1];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self processTouches:touches phase:2];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self processTouches:touches phase:2];
}

- (void)processTouches:(NSSet<UITouch *> *)touches phase:(int)phase {
    CGFloat scale = self.contentScaleFactor;
    int idx = 0;
    for (UITouch *touch in touches) {
        CGPoint pt = [touch locationInView:self];
        TouchHandler_handleTouch(idx, pt.x * scale, pt.y * scale, phase);
        idx++;
    }
}


bool flat = true;
- (void)deviceOrientationDidChange:(NSNotification *)notification {
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    switch (orientation) {
        case UIDeviceOrientationPortrait:
        case UIDeviceOrientationPortraitUpsideDown:
            flat=true;
            break;
        case UIDeviceOrientationLandscapeLeft:
        case UIDeviceOrientationLandscapeRight:
            flat =false;
            break;
        default:
            flat=false;
            break;
    }
    ReloadFrameBuffers();
}


@end


@interface TouchUIViewController : UIViewController
@end


@implementation TouchUIViewController

- (BOOL)prefersHomeIndicatorAutoHidden{
    return true;
}
- (UIViewController*)childViewControllerForHomeIndicatorAutoHidden{
    return nullptr;
}
- (UIRectEdge)preferredScreenEdgesDeferringSystemGestures {
    return UIRectEdgeAll;
}
@end

CustomMTKView *view;
TouchUIViewController *ctrl;

void* touchableMTKView(void* pframe, void* pdevice, void* pdelegate) {
    CGRect frame = *(CGRect*)(pframe);
    view = [[CustomMTKView alloc] initWithFrame:frame device:(__bridge id<MTLDevice>)pdevice];
    
    view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    view.clearColor = MTLClearColorMake(0.1, 0.1, 0.1, 1.0);
    view.depthStencilPixelFormat = MTLPixelFormatDepth16Unorm;
    view.clearDepth = 1.0f;
    
    
    view.multipleTouchEnabled = YES;

    
   // view.delegate = (__bridge id<MTKViewDelegate>)pdelegate;
    
    return (__bridge void*)view;
}

void* touchableViewController() {
    ctrl = [[TouchUIViewController alloc] initWithNibName:nil bundle:nil];
    return (__bridge void*)ctrl;
}
int metalLayerWidth() {
    CGFloat scale = [UIScreen mainScreen].scale;
    return (flat? UIScreen.mainScreen.bounds.size.width: UIScreen.mainScreen.bounds.size.height) * scale; //view.drawableSize.width;
}

int metalLayerHeight() {
    CGFloat scale = [UIScreen mainScreen].scale;
    return (flat? UIScreen.mainScreen.bounds.size.height: UIScreen.mainScreen.bounds.size.width) * scale; //view.drawableSize.height;
}
