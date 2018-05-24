#include "mac_service.h"
#import "hotkeys.h"
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#include <QString>


@implementation Dummy
- (void)runApp:(NSPasteboard *)pboard
                userData:(NSString *)data
                   error:(NSString **)error
{

}
@end

void registerService() {
    Dummy * d = [[Dummy alloc] init];
    NSRegisterServicesProvider(d, @"silverkey-qt");
    NSUpdateDynamicServices();

}
