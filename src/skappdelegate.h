#ifndef SKAPPDELEGATE_H
#define SKAPPDELEGATE_H

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#include "mainwindow.h"

@interface SKObserver : NSObject
- (void)setPreviousApplication:(NSNotification *)aNotification;
@end


@interface SKAppDelegate : NSObject
{
    SKObserver * _nc;
    MainWindow * _mainWindow;
}
- (void)setMainWindow:(MainWindow *)mw;
- (void)savePreviouslyActiveWindow:(NSString *)bID;
@end

void initSKAppDelegate();


#endif // SKAPPDELEGATE_H
