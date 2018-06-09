#ifndef FOCUSCONTROLLER_MAC_H
#define FOCUSCONTROLLER_MAC_H

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

@interface FocusControllerMac: NSObject {
    NSString *_oldAppID;

}
-(void) switchFocusToOld;
-(void) savePrevActive;
-(void) sendToFront;
-(void) eraseOldAppPtr;
@end


#endif // FOCUSCONTROLLER_MAC_H
