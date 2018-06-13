#import "skappdelegate.h"
#import "skappdelegate-c-interface.h"
#import <objc/runtime.h>
#import "mac_service.h"
#include <QString>


@implementation SKObserver
- (id)init
{
    NSNotificationCenter *nc = [[NSWorkspace sharedWorkspace] notificationCenter];
    [nc addObserver:self
                    selector:@selector(setPreviousApplication:)
                    name: NSWorkspaceDidDeactivateApplicationNotification
                    object: nil];
    return [super init];
}
- (void)setPreviousApplication:(NSNotification *)aNotification
{
    NSLog(@"Got notification: %@",  aNotification);
    SKAppDelegate *dlg = [[NSApplication sharedApplication] delegate];
    NSRunningApplication *oApp = [aNotification userInfo][@"NSWorkspaceApplicationKey"];
    [dlg savePreviouslyActiveWindow:oApp.bundleIdentifier];
}
@end

@implementation SKAppDelegate
- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
    _nc = [[SKObserver alloc] init];
    NSLog(@"App WILL FINISH launching");
    NSString *_oldAppID = [[NSWorkspace sharedWorkspace] frontmostApplication].bundleIdentifier;
    [self savePreviouslyActiveWindow:_oldAppID];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSLog(@"APP Finished Launching");
    [NSApp setServicesProvider:[[Dummy alloc] init]];
}

- (void)savePreviouslyActiveWindow:(NSString *)bID
{
    _mainWindow->savePreviouslyActiveWindow(QString::fromNSString(bID));
}

- (void)setMainWindow:(MainWindow *)mw {
    NSLog(@"Assign main window to Delegate");
    _mainWindow = mw;
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    NSLog(@"App DID become active");
    _mainWindow->show();
}
@end

void appLaunched(id self, SEL _cmd)
{
    NSLog(@"APP Finished Launching");
}

void initSKAppDelegate(MainWindow *mw)
{
    NSLog(@"Created a new appdelegate");
    Class cls = [[[NSApplication sharedApplication] delegate] class];
    NSLog(@"Delegate class %@", cls);
    SKAppDelegate *dlg = [SKAppDelegate new];
    [dlg setMainWindow:mw];
    [NSApp setDelegate:dlg];
}
