//
//  toolbarViewMatrix.m
//  DCT
//
//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "toolbarViewMatrix.h"
#import "SMSViewCtrl.h"
#import "DCTAppDelegate.h"

@implementation toolbarViewMatrix

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)mouseDown:(NSEvent *)theEvent
{
	NSInteger row, column;
	NSPoint point = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	
	[self getRow:&row column:&column forPoint:point];
	
	DCTAppDelegate *delegate = (DCTAppDelegate*)TheDelegate;
	
	SMSViewCtrl *smsCtrl = [delegate getSmsViewController];
	
	switch (column)
	{
		case ID_SMS_NEW:
			[smsCtrl smsNew:nil];
			break;
			
		case ID_SMS_REPLY:
			[smsCtrl smsReply:nil];
			break;
			
		case ID_SMS_FORWARD:
			[smsCtrl smsForward:nil];
			break;
			
		case ID_SMS_CUT:
			[smsCtrl smsCut:nil];
			break;
			
		case ID_SMS_COPY:
			[smsCtrl smsCopy:nil];
			break;
			
		case ID_SMS_PASTE:
			[smsCtrl smsPaste:nil];
			break;
			
		case ID_SMS_DELETE:
			[smsCtrl smsDelete:nil];
			break;
			
        case ID_SMS_DOWNLOAD:
        [smsCtrl smsDownload:nil];
			break;
			
		default:
			break;
	}
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	[self updateImage:YES withEvent:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	[self updateImage:NO withEvent:theEvent];
}

- (void)updateImage:(BOOL)bEnter withEvent:(NSEvent *)theEvent
{
	NSInteger row, column;
	NSPoint point = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	
	[self getRow:&row column:&column forPoint:point];
	
	NSButtonCell *btnCell = (NSButtonCell*)[self cellAtRow:row column:column];
	if(btnCell)
	{
		if([btnCell isEnabled])
		{
			if(bEnter)
				[btnCell setImage:[NSImage imageNamed:hotToolbarImages[column]]];
			else
				[btnCell setImage:[NSImage imageNamed:enableToolbarImages[column]]];
		}
	}
}

@end
