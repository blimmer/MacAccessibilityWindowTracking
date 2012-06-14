// A *very* simple app for tracking window events. Good for proof-of-concept purposes only.
//
//
//  Created by l1m5 on 6/13/12.
//

#include <iostream>
#include <ApplicationServices/ApplicationServices.h>

void GenericObserverCallback( AXObserverRef observer, AXUIElementRef element, CFStringRef notificationName, void* contextData)
{
    CFTypeRef windowPosition;
    CGPoint windowPoint;
    
    if (CFStringCompare(notificationName, kAXWindowMiniaturizedNotification, 0) == 0) {
        printf("window was minimized\n");
    } else if (CFStringCompare(notificationName, kAXWindowMovedNotification, 0) == 0) {
        AXUIElementCopyAttributeValue(element, kAXPositionAttribute, &windowPosition);
        AXValueGetValue((AXValueRef) windowPosition, kAXValueCGPointType, (void*) &windowPoint);
        printf ("windows new position is x: %f and y: %f\n", windowPoint.x, windowPoint.y);
    }
}

int main(int argc, const char * argv[])
{
    if (!AXAPIEnabled()) {
        printf("turn on accessibility 'enable access for assistive devices'\n");
        return -1;
    }
    
    
    // gets us a list of running apps and their associated pids
    //CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionAll | kCGWindowListExcludeDesktopElements, kCGNullWindowID);
    //CFRelease(windows);
    
    // change to the appropriate PID for the app in question
    int pid = 1215;
    AXUIElementRef videoConfAppReference = AXUIElementCreateApplication(pid);
    
    // get the front-most window of the app in question
    AXUIElementRef frontVideoConfWindow = NULL;
    if (AXUIElementCopyAttributeValue(videoConfAppReference, kAXMainWindowAttribute, (CFTypeRef *)&frontVideoConfWindow) != kAXErrorSuccess)
    {
        printf("oh noez! couldn't get main window for the provided pid\n");
        return -1;
    }
    
    // create yo observer, son.
    AXObserverRef observer = NULL;
    if (AXObserverCreate(pid, GenericObserverCallback, &observer) != kAXErrorSuccess) {
        printf("oh noez! couldn't create the observer\n");
        return -1;
    }
    
    // sign up for notifications you want
    // see https://developer.apple.com/library/mac/#documentation/Accessibility/Reference/AccessibilityLowlevel/AXNotificationConstants_h/
    AXObserverAddNotification( observer, frontVideoConfWindow, kAXWindowMovedNotification, nil);
    AXObserverAddNotification( observer, frontVideoConfWindow, kAXWindowMiniaturizedNotification, nil);
    
    // start observing!
    CFRunLoopAddSource(CFRunLoopGetCurrent(), AXObserverGetRunLoopSource(observer), kCFRunLoopDefaultMode);
    CFRunLoopRun();
        
    return 0;
}

