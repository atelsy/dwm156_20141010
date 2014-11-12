//
//  RefreshOperDlg.mm
//  DCT
//
//  Created by MBJ on 11-12-27.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "promptDialog.h"
#import "utility.h"

@implementation promptDialog

- (void)windowDidLoad
{
	 [super windowDidLoad];
	 [[self window] setTitle:LocalizeString(@"IDS_PROMPT_CAPTION")];
	 [textMessage setStringValue:LocalizeString(@"IDS_PROMPT_WAITING")];
}

- (void)closePrompt
{
	[NSApp abortModal];
}

@end
