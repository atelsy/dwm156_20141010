//
//  pbYesNoAllDlg.h
//  DCT
//
//  Created by mbj on 12-3-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "ComAgent.h"

#define OPT_YES			0x0100
#define OPT_NO			0x0200
#define OPT_YESALL		0x0400
#define OPT_NOALL		0x0800

#define OPT_YESNO		(OPT_YES|OPT_NO)
#define OPT_YESNOALL	(OPT_YES|OPT_NO|OPT_YESALL|OPT_NOALL)

@interface pbYesNoAllDlg : NSWindowController {
	
	IBOutlet NSTextField *m_ctlText;
	
	NSString *m_strText;
	UINT	m_uMask;
	BOOL	m_bClickBtn;
}

- (IBAction)OnYes:(id)sender;
- (IBAction)OnNo:(id)sender;
- (IBAction)OnYesAll:(id)sender;
- (IBAction)OnNoAll:(id)sender;

- (void)Init:(NSString*)strText withMask:(UINT)uMask;
- (void)CloseWindow:(UINT)code;

@end
