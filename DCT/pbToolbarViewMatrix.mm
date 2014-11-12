//
//  pbToolbarViewMatrix.mm
//  DCT
//
//  Created by mbj on 12-3-10.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbToolbarViewMatrix.h"
#import "pbViewControl.h"
#import "DCTAppDelegate.h"
#import "SMSViewCtrl.h"
@implementation pbToolbarViewMatrix
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
	
	pbViewControl *pbView = [delegate getPbViewController];
   // SMSViewCtrl *smsCtrl = [delegate getSmsViewController];
	
	if([pbView getWindow] != nil)
		return;
	
	switch (column)
	{
		case ID_PB_NEW:
			[pbView OnPbNew:nil];
			break;
			
		case ID_PB_CUT:
			[pbView OnEditCut:nil];
			break;
			
		case ID_PB_COPY:
			[pbView OnEditCopy:nil];
			break;
			
		case ID_PB_PASTE:
			[pbView OnEditPaste:nil];
			break;
			
		case ID_PB_DELETE:
			[pbView OnEditClear:nil];
			break;
			
		case ID_PB_DOWNLOAD:
			[pbView ShowReadDlg:DOWNLOAD];
			break;
		case ID_PB_FILE:
			[pbView OnPbCSV];
			break;
        // case ID_PB_NEWSMS:
        //   [smsCtrl smsNew:nil];
        //   break;
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
