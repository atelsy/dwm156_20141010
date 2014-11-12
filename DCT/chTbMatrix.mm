//
//  chTbMatrix.m
//  DCT
//
//  Created by mtk on 12-6-7.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chTbMatrix.h"
#import "callHistoryViewController.h"
#import "DCTAppDelegate.h"

@implementation chTbMatrix

- (id)init
{
	self = [super init];
	
	if(self)
	{
		
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
	NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	
	[self getRow:&row column:&column forPoint:pt];
	
	DCTAppDelegate *delegate = TheDelegate;
	callHistoryViewController *pView = delegate.chController;
	
	switch (column)
	{
		case ID_CH_CALL:
			[pView onDialCall:nil];
			
			break;
			
		case ID_CH_SENDMSG:
			[pView onSendMessage:nil];
			
			break;
			
		case ID_CH_SAVENUM:
			[pView onSaveNum:nil];
			
			break;
			
		case ID_CH_CLEAN:
			[pView onEditClear:nil];
			
			break;
			
		case ID_CH_DOWNLOAD:
			[pView onDownload:nil];
			
			break;
			
		default:
			break;
	}
}

@end
