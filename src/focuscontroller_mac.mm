#include "focuscontroller.h"
#include "focuscontroller_mac.h"

@implementation FocusControllerMac


-(void) switchFocusToOld
{
    NSLog(@"Switching to %@", self->_oldAppID);
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    NSArray * apps = [ws runningApplications];
    for (NSRunningApplication *app in apps)
    {
        if ([self->_oldAppID isEqualToString:app.bundleIdentifier])
        {
            NSLog(@"Found app %@", app);
            [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
            //self->_oldAppID = nil;
            break;
        }
    }
}

-(void) savePrevActive
{
    self->_oldAppID = [[NSWorkspace sharedWorkspace] frontmostApplication].bundleIdentifier;
    NSLog(@"Saving previous focus %@", self->_oldAppID);

}

-(void) sendToFront
{
    [NSApp activateIgnoringOtherApps:YES];
}

-(void) eraseOldAppPtr
{
    self->_oldAppID = nil;
}
@end

FocusController::FocusController(QObject *parent) :
    QObject(parent)
{
    controller = [[FocusControllerMac alloc] init];
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    [ctrl savePrevActive];
}



void FocusController::switchFocus()
{
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    [ctrl switchFocusToOld];
}

void FocusController::savePrevActive() {
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    [ctrl savePrevActive];
}

void FocusController::sendToFront()
{
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    [ctrl sendToFront];
}

void FocusController::eraseOldAppPtr()
{
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    [ctrl eraseOldAppPtr];
}

FocusController::~FocusController()
{
    delete &controller;
}
