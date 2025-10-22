
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>


#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

CAMetalLayer* metalLayer = nil;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    metalLayer.drawableSize = CGSizeMake(width, height);
}

void apphide_togglehide(GLFWwindow *wnd, bool state = true) {
   glfwSetWindowAttrib( wnd, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE );
   glfwSetWindowAttrib( wnd, GLFW_DECORATED, GLFW_FALSE );
   glfwSetWindowAttrib( wnd, GLFW_FOCUS_ON_SHOW, GLFW_FALSE );
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    
    glfwSetWindowSize(wnd, mode->width, mode->height);
    
   ProcessSerialNumber psn = {0, kCurrentProcess};
   TransformProcessType(&psn, kProcessTransformToUIElementApplication);

   NSWindow* ns_wnd = glfwGetCocoaWindow(wnd);
   ns_wnd.level = kCGDesktopIconWindowLevel;
   ns_wnd.collectionBehavior = NSWindowCollectionBehaviorCanJoinAllSpaces;

   [ns_wnd setFrame:CGRectMake(0, 0, [ns_wnd frame].size.width , [ns_wnd frame].size.height) display:YES];

}

void apphide(GLFWwindow* wnd, void* metalDevice) {

    //apphide_togglehide(wnd);
    
    int width, height;
    glfwGetFramebufferSize(wnd, &width, &height);

    NSWindow* metalWindow = glfwGetCocoaWindow(wnd);
    metalLayer = [CAMetalLayer layer];
    metalLayer.device = (__bridge id<MTLDevice>)metalDevice;
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.drawableSize = CGSizeMake(width, height);
    metalWindow.contentView.layer = metalLayer;
    metalWindow.contentView.wantsLayer = YES;

    printf("Metal layer set up\n");
    
    glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);

}

int metalLayerWidth() {
    return metalLayer.drawableSize.width;
}

int metalLayerHeight() {
    return metalLayer.drawableSize.height;
}

void* contentView(GLFWwindow* wnd) {
    NSWindow* metalWindow = glfwGetCocoaWindow(wnd);
    return (__bridge void*)metalWindow.contentView;
}


void* currentDrawable() {           
    return (__bridge void*)[metalLayer nextDrawable];
}
