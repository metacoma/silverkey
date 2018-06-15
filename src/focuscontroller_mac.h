#ifndef FOCUSCONTROLLER_MAC_H
#define FOCUSCONTROLLER_MAC_H

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

class FocusControllerMac
{
public:
    FocusControllerMac();
    void switchFocusToOld();
    void savePrevActive();
private:
    NSRunningApplication *_oldApp;
};


#endif // FOCUSCONTROLLER_MAC_H
