//
//  RefreshOperDlg.h
//  DCT
//
//  Created by MBJ on 11-12-27.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface promptDialog : NSWindowController {
	IBOutlet NSTextField*	textMessage;
}

- (void)closePrompt;

@end
