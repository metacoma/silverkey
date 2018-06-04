#include "focuscontroller.h"
#include "focuscontroller_mac.h"


FocusControllerMac::FocusControllerMac()
{
    NSLog(@"%@", [[NSWorkspace sharedWorkspace] frontmostApplication].bundleIdentifier);
    _oldApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
}

void FocusControllerMac::switchFocusToOld()
{
    NSLog(@"%@", [_oldApp isActive]);
    [_oldApp activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    NSLog(@"%@", [_oldApp isActive]);
}

FocusController::FocusController(QObject *parent) :
    QObject(parent)
{
    controller = new FocusControllerMac();
}

void FocusController::switchFocus()
{
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    ctrl->switchFocusToOld();
}

FocusController::~FocusController()
{
    delete &controller;
}
