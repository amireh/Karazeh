//
//  CAppAppDelegate.h
//  CApp
//
//  Created by Ahmad Amireh on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Repository.h"

@interface KAppDelegate : NSObject <NSApplicationDelegate> {
  NSWindow *window;
  NSButton *btnLaunch;
  NSProgressIndicator *progressBar;
  NSTextView *txtLatestChanges;
  NSTextField *txtStatus;  
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSTextView *txtLatestChanges;
@property (assign) IBOutlet NSProgressIndicator *progressBar;
@property (assign) IBOutlet NSButton *btnLaunch;
@property (assign) IBOutlet NSTextField *txtStatus;

typedef const Pixy::Version Version;

-(IBAction) clickLaunch: (id) sender;

-(void) unableToConnect;
-(void) validateStarted;
-(void) showPatchLog: (NSString*) inLogPath;
-(void) validateComplete: (BOOL) inNeedUpdate : (Version&) inVersion;
-(void) patchStarted: (Version&) inVersion;
-(void) patchProgress: (double)inPercent;
-(void) patchFailed: (NSString*)inMsg : (Version&) inVersion;
-(void) patchComplete: (Version&) inVersion;
-(void) applicationPatched: (Version&) inVersion;

@end