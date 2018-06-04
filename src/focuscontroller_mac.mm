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

void FocusControllerMac::savePrevActive()
{
    _oldApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
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

void FocusController::savePrevActive() {
    FocusControllerMac *ctrl = (FocusControllerMac *) controller;
    ctrl->savePrevActive();
}

FocusController::~FocusController()
{
    delete &controller;
}
