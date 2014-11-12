//
//  pbYesNoAllDlg.mm
//  DCT
//
//  Created by mbj on 12-3-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbYesNoAllDlg.h"

@implementation pbYesNoAllDlg

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
}

-(void)awakeFromNib
{	
	[m_ctlText setStringValue:m_strText];
}

- (void)Init:(NSString*)strText withMask:(UINT)uMask
{
	m_strText = [NSString stringWithString:strText];
	m_uMask = uMask;
	m_bClickBtn = FALSE;
}

- (void)CloseWindow:(UINT)code
{
	m_bClickBtn = TRUE;
	[NSApp stopModalWithCode:code];
	[[self window] performClose:nil];
}

- (IBAction)OnYes:(id)sender
{
	[self CloseWindow:OPT_YES];
}

- (IBAction)OnNo:(id)sender
{
	[self CloseWindow:OPT_NO];
}

- (IBAction)OnYesAll:(id)sender
{
	[self CloseWindow:OPT_YESALL];
}

- (IBAction)OnNoAll:(id)sender
{
	[self CloseWindow:OPT_NOALL];
}

- (BOOL)windowShouldClose:(id)sender
{
	if (!m_bClickBtn)
	{
		[NSApp stopModalWithCode:OPT_NOALL];
	}
	return YES;
}

@end
