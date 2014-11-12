//
//  smsListView.m
//  DCT
//  override tableview to deal with keyboard event

//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsListView.h"
#import "smsListCtrl.h"
#import "DCTAppDelegate.h"
#import "SMSViewCtrl.h"

@implementation smsListView

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
	smsListCtrl *delegate = (smsListCtrl*)[self delegate];
	SMSViewCtrl *pView = (SMSViewCtrl*)[delegate getParent];
	
	if(key == NSDeleteCharacter)
	{
		[pView smsDelete:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"a"] || [[theEvent characters] isEqualToString:@"A"]))
	{
		[self selectAll:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"x"] || [[theEvent characters] isEqualToString:@"X"]))
	{
		[pView smsCut:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"c"] || [[theEvent characters] isEqualToString:@"C"]))
	{
		[pView smsCopy:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"v"] || [[theEvent characters] isEqualToString:@"V"]))
	{
		[pView smsPaste:nil];
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

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSLog(@"mouse move into list ctrl.");
}

@end
