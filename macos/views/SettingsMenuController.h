//
//  SettingsMenuController.h
//  mkxp-z
//
//  Created by ゾロアーク on 1/15/21.
//

#ifndef MKXPZ_SETTINGSMENUCONTROLLER_H
#define MKXPZ_SETTINGSMENUCONTROLLER_H

#import <vector>
#import <AppKit/AppKit.h>

#import "eventthread.h"

@interface SettingsMenu : NSViewController <NSTableViewDelegate, NSTableViewDataSource>
+(SettingsMenu *)openWindow;
-(SettingsMenu *)setWindow:(NSWindow *)window;
-(SettingsMenu *)raise;
@end

#endif // MKXPZ_SETTINGSMENUCONTROLLER_H
