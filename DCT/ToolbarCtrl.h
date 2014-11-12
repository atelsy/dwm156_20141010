//
//  ToolbarCtrl.h
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DCTAppDelegate.h"


@interface ToolbarCtrl : NSWindowController <NSToolbarDelegate>
{
@private
	IBOutlet NSToolbar *toolbar;

	NSMutableArray *moduleIDList;

}

- (IBAction)loadCallView:(id)sender;
- (IBAction)loadCallHistoryView:(id)sender;
- (IBAction)loadPhonebookView:(id)sender;
- (IBAction)loadSMSView:(id)sender;
- (IBAction)loadDialupview:(id)sender;
- (IBAction)loadStatisticsView:(id)sender;
- (IBAction)showUssd:(id)sender;

- (void)modifyToolbarItemStatus:(NSInteger)selectedTag;

@end
