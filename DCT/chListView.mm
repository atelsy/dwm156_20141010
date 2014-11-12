//
//  chListView.m
//  DCT
//
//  Created by mtk on 12-7-6.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "chListView.h"
#import "DCTAppDelegate.h"
#import "callHistoryViewController.h"

@implementation chListView

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

- (void)keyDown:(NSEvent *)theEvent
{
	unichar key = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	DCTAppDelegate *delegate = (DCTAppDelegate*)[NSApp delegate];
	callHistoryViewController *pView = delegate.chController;
	
	if(key == NSDeleteCharacter)
	{
		[pView onEditClear:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"a"] || [[theEvent characters] isEqualToString:@"A"]))
	{
		[self selectAll:nil];
	}
	else
	{
		[super keyDown:theEvent];
	}
}

- (NSMenu*)menuForEvent:(NSEvent *)event
{
	if([event type] == NSRightMouseDown)
	{
		if([self isEnabled] == NO)
			return nil;
	}
	
	return [super menuForEvent:event];
}

@end
