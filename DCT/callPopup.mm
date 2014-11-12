//
//  callPopup.m
//  DCT
//
//  Created by MBJ on 12-6-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "callPopup.h"
#import "ComAgent.h"
#import "DCTAppDelegate.h"
#import "callControl.h"
#import "nsstrex.h"

@implementation CallPopup

- (id)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window])
	{
		theCall = NULL;
		engine  = nil;
	}
	return self;
}

- (void)dealloc
{
	if (theCall != NULL)
	{
		delete theCall;
	}
	
	[super dealloc];
}

- (IBAction)onAnswer:(id)sender
{
	[owner onCallCtrl:CC_BTN_ANSWER index:0];
	
	[TheDelegate loadCallView:nil];
}

- (IBAction)onReject:(id)sender
{
	[owner onCallCtrl:CC_BTN_REJECT index:0];
}

- (void)bindCall:(const CCall*)pCall
	   andEngine:(CallEngine*)en9ine
{
	ASSERT(theCall == NULL);
	theCall = new CCall(pCall);
	engine = en9ine;
}

- (void)awakeFromNib
{
	CString &_alpha = theCall->alpha;
	CString &_number = theCall->number;
	
	NSString *strAlpha = nil;
	NSString *strNumber = nil;
	
	if (_alpha.IsEmpty())
	{
		strAlpha = LoadString(@"IDS_CC_UNKNOWN_CONTACT");
	}
	else
	{
		strAlpha = [NSString CStringToNSString:_alpha];
	}
	
	if (_number.IsEmpty())
	{
		strNumber = LoadString(@"IDS_CC_UNKNOWN_NUMBER");
	}
	else
	{
		strNumber = [NSString CStringToNSString:_number];
	}

	[alpha setStringValue:strAlpha];
	[number setStringValue:strNumber];
	
	if (theCall->IsWaiting())
	{
		[iconIncm setImage:[NSImage imageNamed:@"waiting_call.gif"]];
		[label setStringValue:@"Waiting call:"];
	}
	else
	{
		[iconIncm setImage:[NSImage imageNamed:@"incoming_call.gif"]];
	}

	
	[self updateCtrl];
}

- (void)showWindow:(callControl*)owNer
			origin:(NSPoint)origin
{
	owner = owNer;
	
	[self.window setFrameOrigin:origin];
	
	[self.window orderFront:owner];
	
//	[self.window setParentWindow:nil];
	
	[self showWindow:nil];
}

- (void)updateCtrl
{
	[btnAnswer setEnabled:([engine AnswerTest] == CA_OK)];
	[btnReject setEnabled:([engine RejectTest] == CA_OK)];
}

@end
